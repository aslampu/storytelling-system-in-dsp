%% Load Image
% Read BackGround Raw File From Lab Camera
clear all; close all;clc;
fid = fopen('output.raw', 'rb');
bg_matrix = fread(fid, 'uchar');

%% Load Test Insert Images
insert_image = imread('test_insert.png');
%% Transfer Raw File Format (230400x1) To Image File Format (320x240x3)
bg_image = zeros(240,320,3);
for i = 1:240
    for j = 1:320
        for k = 1:3
            index = 1 + 3*(j-1) + (i-1)*320*3 + (k-1);
                bg_image(i,j,k) = (bg_matrix(index,1));
        end
    end
end

% Define Shadow structure parameters
shadow.x = 0;
shadow.y = 0;
shadow.width = 20;
shadow.height = 10;



%% Display Comparsion
figure; hold on;
subplot(2,2,1); image(new_bg_rgb), title('Background');
subplot(2,2,2); image(new_bg_rgb_with_origin), title('Origina Image');
subplot(2,2,3); image(new_bg_rgb_with_adjust), title('Adjust Image');
subplot(2,2,4); image(new_bg_rgb_with_gussian),title('Gaussian Image');