#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

// 保存图像的像素结构
typedef struct {
    uint8_t r, g, b, a;
} pixel;

// 生成曼德尔布罗特集合
void mandelbrot(pixel* image) {
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double real = (x * 4.0 / WIDTH) - 2.0; // X轴映射到 [-2, 2]
            double imag = (y * 4.0 / HEIGHT) - 2.0; // Y轴映射到 [-2, 2]
            double r = 0.0, i = 0.0;

            int n;
            for (n = 0; n < MAX_ITER; n++) {
                if (r * r + i * i > 4.0) break;
                double temp = r * r - i * i + real;
                i = 2 * r * i + imag;
                r = temp;
            }

            // 设置像素颜色
            pixel p;
            if (n == MAX_ITER) {
                p = (pixel){0, 0, 0, 255}; // 黑色
            } else {
                p = (pixel){n % 256, n % 256, n % 256, 255}; // 颜色深度渐变
            }
            image[y * WIDTH + x] = p;
        }
    }
}

// 保存图像
void save_image(pixel* image, const char* filename) {
    FILE* f = fopen(filename, "wb");
    fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        fwrite(&image[i], sizeof(pixel), 1, f);
    }
    fclose(f);
}

int main() {
    pixel* image = (pixel*)malloc(WIDTH * HEIGHT * sizeof(pixel));

    // 开始计时
    double start_time = omp_get_wtime();
    
    // 生成曼德尔布罗特集合
    mandelbrot(image);
    
    // 结束计时
    double end_time = omp_get_wtime();
    printf("Execution time: %f seconds\n", end_time - start_time);
    
    // 保存图像
    save_image(image, "mandelbrot.ppm");

    free(image);
    return 0;
}