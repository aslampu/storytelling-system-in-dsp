%% Load Image
% Read BackGround Raw File From Lab Camera
clear all; close all;clc;
fid = fopen('output.raw', 'rb');
bg_matrix = fread(fid, 'uchar');

%% Transfer Raw File Format (230400x1 - RGBRGBRGB...) To Image File Format (320x240x3)
bg_imageRGB = zeros(240,320,3);
for i = 1:240
    for j = 1:320
        for k = 1:3
            index = 1 + 3*(j-1) + (i-1)*320*3 + (k-1);
                bg_imageRGB(i,j,k) = (bg_matrix(index,1));
        end
    end
end

%% Obtain HSV Colormap From bg_imageRGB
bg_imageHSV = rgb2hsv(bg_imageRGB./255);

%% Calculate Mean and Std Dev. of Background HSV Channels
bg_H_array = reshape(bg_imageHSV(:,:,1), 1, 240*320);
bg_S_array = reshape(bg_imageHSV(:,:,2), 1, 240*320);
bg_V_array = reshape(bg_imageHSV(:,:,3), 1, 240*320);

bg_H_array = bg_H_array.*360;
bg_S_array = bg_S_array.*100;
bg_V_array = bg_V_array.*100;

bg_H_mean = mean(bg_H_array);
bg_S_mean = mean(bg_S_array);
bg_V_mean = mean(bg_V_array);
bg_H_std = std(double(bg_H_array));
bg_S_std = std(double(bg_S_array));
bg_V_std = std(double(bg_V_array));

%% Display HSV Histogram
figure(1); 
hist(bg_H_array, 0:1:360); title('H Channel');
figure(2);
hist(bg_S_array, 0:1:100);  title('S Channel');
figure(3); 
hist(bg_V_array, 0:1:100); title('V Channel');