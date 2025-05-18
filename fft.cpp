//
// sudo apt-get install libfftw3-dev
// g++ -o fft_example fft_example.cpp -lfftw3 -lm
//
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fftw3.h>
#include "fft.h"

const double Fs = 1024000; //1e6; // Sample rate
const int N = 1024;    // FFT size

//int main() {
int fft_psd(std::vector<std::complex<float>> samples) {
        // Assume x contains your array of IQ samples
    std::vector<std::complex<double>> x(N);
    //printf("%ld\n", samples.size());
    
    // Fill x with your IQ samples (for demonstration, we use random values)
    for (int i = 0; i < N; ++i) {
        //x[i] = std::complex<double>(rand() / double(RAND_MAX), rand() / double(RAND_MAX));
        //x[i] = std::complex<double>(samples[i].real(), samples[i].imag());
    }

    // Create FFTW plan
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Copy data to input array
    for (int i = 0; i < N; ++i) {
        //in[i][0] = x[i].real();  // Real part
        //in[i][1] = x[i].imag();  // Imaginary part
        in[i][0] = samples[i].real();  // Real part
        in[i][1] = samples[i].imag();  // Imaginary part
        //printf("%f %f\n", samples[i].real(), samples[i].imag());
    }

    // Execute FFT
    fftw_execute(p);

    // Calculate Power Spectral Density (PSD)
    std::vector<double> PSD(N);
    for (int i = 0; i < N; ++i) {
        double magnitude = std::sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        PSD[i] = (magnitude * magnitude) / (N * Fs); // Power
    }

    // Convert to dB
    std::vector<double> PSD_log(N);
    for (int i = 0; i < N; ++i) {
        PSD_log[i] = 10.0 * std::log10(PSD[i]);
    }

    // FFT shift
    std::vector<double> PSD_shifted(N);
    int half_N = N / 2;
    for (int i = 0; i < half_N; ++i) {
        PSD_shifted[i] = PSD_log[i + half_N];
        PSD_shifted[i + half_N] = PSD_log[i];
    }

    // Clean up
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    float avg = 0.0f;
    for (uint8_t i=25; i < 50; i++)
    {
        avg += PSD_log[i];
    }
    avg /= 25;
#if 0
    // Output the shifted PSD for demonstration
    for (uint8_t idx = 0; idx < 10; idx++)
    {
        //printf("%f ", PSD_shifted[idx]);
        //printf("%f ", PSD_log[idx]);
        
    }
    printf("FFT SNR(%f)  %f  %f\n", PSD_log[0]-avg, PSD_log[0], avg);
    //printf("\n");
#endif // 0
#if 0
    for (const auto& value : PSD_shifted) {
        std::cout << value << std::endl;
    }
#endif // 0

    return (int)(PSD_log[0]-avg);
}
