import numpy as np
import pyopencl as cl
import time
import matplotlib.pyplot as plt

# 设置计算曼德尔布罗特集合的参数
width, height = 800, 800
max_iter = 255
x_min, x_max = -2.0, 1.0
y_min, y_max = -1.5, 1.5


# 生成坐标数组
def mandelbrot_set_parallel(width, height, max_iter, x_min, x_max, y_min, y_max):
    # 初始化OpenCL
    context = cl.create_some_context()
    queue = cl.CommandQueue(context)

    # 创建输入数据
    x = np.linspace(x_min, x_max, width).astype(np.float32)
    y = np.linspace(y_min, y_max, height).astype(np.float32)
    x, y = np.meshgrid(x, y)
    x = x.flatten()
    y = y.flatten()

    # 创建输出数组
    output = np.zeros(x.shape, dtype=np.uint8)

    # OpenCL内核代码
    kernel_code = """
    __kernel void mandelbrot(__global const float *x, __global const float *y,
                              __global unsigned char *output, int max_iter) {
        int gid = get_global_id(0);
        float c_re = x[gid];
        float c_im = y[gid];
        float z_re = c_re, z_im = c_im;
        int n;

        for (n = 0; n < max_iter; n++) {
            if (z_re * z_re + z_im * z_im > 4.0) break;
            float new_re = z_re * z_re - z_im * z_im + c_re;
            z_im = 2 * z_re * z_im + c_im;
            z_re = new_re;
        }
        output[gid] = (unsigned char)n;
    }
    """

    # 构建程序
    program = cl.Program(context, kernel_code).build()

    # 分配设备内存
    mf = cl.mem_flags
    x_buf = cl.Buffer(context, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=x)
    y_buf = cl.Buffer(context, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=y)
    output_buf = cl.Buffer(context, mf.WRITE_ONLY, output.nbytes)

    # 启动并行计算
    start_time = time.time()
    program.mandelbrot(queue, output.shape, None, x_buf, y_buf, output_buf, np.int32(max_iter))
    cl.enqueue_copy(queue, output, output_buf)
    queue.finish()
    end_time = time.time()
    parallel_time = end_time - start_time

    return output, parallel_time


# 非并行运算 (CPU)
def mandelbrot_set_non_parallel(width, height, max_iter, x_min, x_max, y_min, y_max):
    output = np.zeros((height, width), dtype=np.uint8)
    x = np.linspace(x_min, x_max, width)
    y = np.linspace(y_min, y_max, height)

    start_time = time.time()
    for i in range(height):
        for j in range(width):
            c_re = x[j]
            c_im = y[i]
            z_re, z_im = c_re, c_im
            n = 0

            while n < max_iter:
                if z_re * z_re + z_im * z_im > 4.0:
                    break
                new_re = z_re * z_re - z_im * z_im + c_re
                z_im = 2 * z_re * z_im + c_im
                z_re = new_re
                n += 1

            output[i, j] = n

    end_time = time.time()
    non_parallel_time = end_time - start_time

    return output, non_parallel_time


# 主程序
def main():
    parallel_result, parallel_time = mandelbrot_set_parallel(width, height, max_iter, x_min, x_max, y_min, y_max)
    non_parallel_result, non_parallel_time = mandelbrot_set_non_parallel(width, height, max_iter, x_min, x_max, y_min,
                                                                         y_max)

    # 绘制结果
    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.title(f'Parallel Execution Time: {parallel_time:.3f}s')
    plt.imshow(parallel_result.reshape((height, width)), cmap='hot')
    plt.axis('off')

    plt.subplot(1, 2, 2)
    plt.title(f'Non-Parallel Execution Time: {non_parallel_time:.3f}s')
    plt.imshow(non_parallel_result, cmap='hot')
    plt.axis('off')

    plt.show()


if __name__ == "__main__":
    main()