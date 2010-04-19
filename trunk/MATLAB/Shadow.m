%% Load Image
% Read BackGround Raw File From Lab Camera
clear all; close all; clc;
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
shadow.x = 100;
shadow.y = 100;
shadow.a = 30;
shadow.b = 30;

% Define bg_image matrices
bg_image_with_shadow = bg_image;
bg_image_hsv = rgb2hsv(bg_image);
[width, height] = size(bg_image);

% Define region of interest
roi.original = bg_image(max(shadow.y, 1):min(shadow.y + shadow.b, height), max(shadow.x - shadow.a, 1):min(shadow.x + shadow.a, width), :);

% Transform image to HSV and put as attribute of roi
roi.hsv = rgb2hsv(roi.original);
roi.hsv_modified = bg_image_hsv;

% Apply shadown to ROI
for i = max(shadow.y - shadow.b, 1):min(shadow.y + shadow.b, height)
  for j = max(shadow.x - shadow.a, 1):min(shadow.x + shadow.a, width)
    roi.hsv_modified(i,j,1) = bg_image_hsv(i,j,1);
    roi.hsv_modified(i,j,2) = bg_image_hsv(i,j,2);
    roi.hsv_modified(i,j,3) = 0.5 * bg_image_hsv(i,j,3);
  end 
end

% Revert HSV to RGB
roi.with_shadow = hsv2rgb(roi.hsv_modified);

bg_image_with_shadow(max(shadow.y, 1):min(shadow.y + shadow.b, height), max(shadow.x - shadow.a, 1):min(shadow.x + shadow.a, width), :) = roi.with_shadow(max(shadow.y, 1):min(shadow.y + shadow.b, height), max(shadow.x - shadow.a, 1):min(shadow.x + shadow.a, width), :);

roi.with_shadow = roi.with_shadow(max(shadow.y, 1):min(shadow.y + shadow.b, height), max(shadow.x - shadow.a, 1):min(shadow.x + shadow.a, width), :);

%% Display Comparsion
figure; hold on;
subplot(2,2,1); colormap('colorcube'); imagesc(bg_image./255), title('Background');
subplot(2,2,2); colormap('gray'); imagesc(roi.original./255), title('ROI');
subplot(2,2,3); imagesc(bg_image_with_shadow./255), title('Adjusted Image');
subplot(2,2,4); imagesc(roi.with_shadow./255), title('Adjusted ROI');
