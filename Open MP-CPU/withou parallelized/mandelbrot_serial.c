#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h> // 包含时间函数的头文件

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

// 定义一个像素结构来保存图像数据
typedef struct {
    uint8_t r, g, b;
} pixel;

// 计算一个像素的曼德尔布罗特集合值
int mandelbrot(double real, double imag) {
    double r = 0.0, i = 0.0;
    int n;
    for (n = 0; n < MAX_ITER; n++) {
        if (r * r + i * i > 4.0) {
            break;
        }
        double temp = r * r - i * i + real;
        i = 2 * r * i + imag;
        r = temp;
    }
    return n;
}

// 生成曼德尔布罗特集合图像数据
void generate_mandelbrot(pixel *image) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double real = (x * 4.0 / WIDTH) - 2.0; // X轴映射到 [-2, 2]
            double imag = (y * 4.0 / HEIGHT) - 2.0; // Y轴映射到 [-2, 2]
            int n = mandelbrot(real, imag);

            // 设置像素颜色
            pixel p;
            if (n == MAX_ITER) {
                p = (pixel){0, 0, 0}; // 黑色
            } else {
                // 根据迭代次数设置颜色
                p = (pixel){n % 256, n % 256, n % 256}; 
            }
            image[y * WIDTH + x] = p;
        }
    }
}

// 保存图像
void save_image(pixel *image, const char *filename) {
    FILE *f = fopen(filename, "wb");
    fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        fwrite(&image[i], sizeof(pixel), 1, f);
    }
    fclose(f);
}

int main() {
    pixel *image = (pixel *)malloc(WIDTH * HEIGHT * sizeof(pixel));
    
    // 记录开始时间
    clock_t start_time = clock();

    // 生成曼德尔布罗特集合图像
    generate_mandelbrot(image);
    
    // 记录结束时间
    clock_t end_time = clock();

    // 计算并输出CPU运算时间
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; // 转换为秒
    printf("计算曼德尔布罗特集合所需时间: %.6f 秒\n", elapsed_time);

    // 保存图像
    save_image(image, "mandelbrot.ppm");

    free(image);
    return 0;
}