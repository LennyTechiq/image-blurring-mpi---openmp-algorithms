#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void readPGM(const char *filename, int **image, int *width, int *height, int *maxval) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    char format[3];
    fscanf(file, "%s", format);
    fscanf(file, "%d %d", width, height);
    fscanf(file, "%d", maxval);

    *image = (int *)malloc(*width * *height * sizeof(int));
    for (int i = 0; i < *width * *height; i++) {
        fscanf(file, "%d", &(*image)[i]);
    }

    fclose(file);
}

void writePGM(const char *filename, int *image, int width, int height, int maxval) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", maxval);

    for (int i = 0; i < width * height; i++) {
        fprintf(file, "%d ", image[i]);
        if ((i + 1) % width == 0) {
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

void blurImage(int *image, int *output, int width, int height) {
    int kernelSize = 3; // 3x3 kernel
    int offset = kernelSize / 2; // 1 (for a 3x3 kernel)

    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int sum = 0;
            int count = 0;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int ny = y + ky;
                    int nx = x + kx;

                    if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                        sum += image[ny * width + nx];
                        count++;
                    }
                }
            }

            output[y * width + x] = sum / count;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input.pgm> <output.pgm>\n", argv[0]);
        return 1;
    }

    int *image, *blurred_image;
    int width, height, maxval;

    // Read input PGM image
    readPGM(argv[1], &image, &width, &height, &maxval);

    // Allocate memory for the blurred image
    blurred_image = (int *)malloc(width * height * sizeof(int));

    // Perform the blur effect 20 times (as per the assignment)
    for (int i = 0; i < 20; i++) {
        blurImage(image, blurred_image, width, height);
        // Swap pointers for next iteration
        int *temp = image;
        image = blurred_image;
        blurred_image = temp;
    }

    // Write the output blurred image
    writePGM(argv[2], image, width, height, maxval);

    // Free allocated memory
    free(image);
    free(blurred_image);

    return 0;
}
