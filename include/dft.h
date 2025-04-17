//
// Created by Samuel Longauer on 22/02/2024.
//

#ifndef FOURIER_TRANSFORM_DFT_H
#define FOURIER_TRANSFORM_DFT_H

#include <iostream>
#include <vector>
#include <complex>
#include <fstream>
#include <cmath>

typedef std::vector<std::complex<double>> cmplx_field;

cmplx_field FFT(cmplx_field & x) {
    int N = x.size();
    if (N <= 1) return x;

    cmplx_field even(N/2), odd(N/2);
    for (int i = 0; i < N/2; ++i) {
        even[i] = x[i*2];
        odd[i] = x[i*2 + 1];
    }

    even = FFT(even);
    odd = FFT(odd);

    cmplx_field result(N);
    for (int k = 0; k < N/2; ++k) {
        std::complex<double> t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        result[k] = even[k] + t;
        result[k + N/2] = even[k] - t;
    }
    return result;
}

cmplx_field IFFT(cmplx_field & x) {
    for (auto& val : x) {
        val = std::conj(val);
    }

    cmplx_field result = FFT(x);

    for (auto& val : result) {
        val = std::conj(val) / static_cast<double>(result.size());
    }

    return result;
}

cmplx_field DFT(cmplx_field & signal){
    int N = (int)signal.size(); //number of samples
    int K = N;

    cmplx_field output;
    output.reserve(N);

    for (size_t k = 0; k<K; ++k){
        std::complex<double> totalSum(0,0);
        for (size_t n = 0; n<N; ++n){
            double Real_component = cos((2*M_PI)/N*k*n);
            double Imag_component = -sin((2*M_PI)/N*k*n);
            std::complex<double> w(Real_component, Imag_component);
            totalSum += signal[n]*w;
        }
        output.emplace_back(totalSum);
    }

    return output;
}

std::vector<double> computeMagnitudes(cmplx_field& cmplx){
    size_t N = cmplx.size();
    std::vector<double> magnitudes(N);
    for (size_t i=0; i<N; ++i){
        double real = cmplx[i].real();
        double imag = cmplx[i].imag();
        magnitudes[i] = sqrt(real*real + imag*imag);
    }
    return magnitudes;
}

// the following spectra are one-sided (only returning scaled values from 0 up to the Nyquist frequency)

std::vector<double> amplitudeSpectrum(cmplx_field& cmplx);

std::vector<double> powerSpectrum(cmplx_field& cmplx);

std::vector<double> powerSpectralDensity(cmplx_field& cmplx, int num_samples, int sample_rate){
    std::vector<double> magnitudes = computeMagnitudes(cmplx);
    std::vector<double> spectrum;
    int upper_bound = ceil((num_samples+1)/2.0);

    spectrum.reserve(upper_bound);
for (int i = 0; i<upper_bound; i++){
        spectrum.push_back(2*magnitudes[i]*magnitudes[i]/num_samples/sample_rate);
    }

    spectrum[0]/=2; // the zero frequency is unique
    if (num_samples%2 == 0) spectrum[upper_bound-1]/=2; // the Nyquist frequency is unique

    return spectrum;

}

// windowing function - for minimizing spectral leakage
void applyHannWindow(std::vector<double>& data) {
    int N = (int)data.size();
    for (int n = 0; n < N; ++n) {
        double windowValue = 0.5 * (1 - std::cos(2 * M_PI * n / (N - 1))); // Hann window formula
        data[n] *= windowValue; // Apply the window
    }
}

#endif //FOURIER_TRANSFORM_DFT_H
