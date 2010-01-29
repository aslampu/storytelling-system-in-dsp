clc
clear all

output = filter(Numerator,Denomenator,input_signal);
soundsc(output)
Y = fft(output);
figure; plot(abs(Y))