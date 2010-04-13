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
shadow.x = 200;
shadow.y = 200;
shadow.a = 40;
shadow.b = 30;

% Define bg_image matrices
bg_image_with_shadow = bg_image;
bg_image_hsv = rgb2hsv(bg_image);
[width, height] = size(bg_image);

bg_imageSize = size(bg_image);

%% Define region of interest
roi.original = zeros(shadow.b*2, shadow.a*2, 3);
for i = 1:1:bg_imageSize(1,2)
    for j = 1:1:bg_imageSize(1,1)
        outcome = ((i - shadow.x)^2)/(shadow.a^2) + ((j - shadow.y)^2)/(shadow.b^2);
        if(outcome <= 1)
            
            roiY = j-(shadow.y-shadow.b);
            roiX = i-(shadow.x-shadow.a);
            
            if roiY == 0
                roiY = 1;
            end
            
            if roiX == 0
                roiX = 1;
            end
            
            roi.original(roiY,roiX,:) = bg_image(j,i,:);
        end
    end
end

%% Transform image to HSV and put as attribute of roi
roi.hsv = rgb2hsv(roi.original);
roi.hsv_modified = bg_image_hsv;

%% Adjust the shadow
eccentricity  = (1-(shadow.b/shadow.a)^2)^(1/2);
distanceFromCenter = shadow.a*eccentricity;
focusXRight = shadow.x + distanceFromCenter;
focusYLeft = shadow.x - distanceFromCenter;

for i = 1:1:bg_imageSize(1,2)
    for j = 1:1:bg_imageSize(1,1)
        outcome = ((i - shadow.x)^2)/(shadow.a^2) + ((j - shadow.y)^2)/(shadow.b^2);
        if(outcome <= 1)
            
            distance = ((i - focusXRight)^2 + (j - shadow.y)^2)^(1/2) + ((i - focusYLeft)^2 + (j - shadow.y)^2)^(1/2);
            
            edge = shadow.a + distanceFromCenter;
            
            adjust = (distance/edge)^2;
            
            if(adjust >= 1)
                adjust = 1;
            end
            
            roi.hsv_modified(j,i,1) = bg_image_hsv(j,i,1);
            roi.hsv_modified(j,i,2) = bg_image_hsv(j,i,2);
            roi.hsv_modified(j,i,3) = adjust * bg_image_hsv(j,i,3);
            
            
            
            
        end
    end
end




%% Revert HSV to RGB
roi.with_shadow = hsv2rgb(roi.hsv_modified);

%%
for i = 1:1:bg_imageSize(1,2)
    for j = 1:1:bg_imageSize(1,1)
        outcome = ((i - shadow.x)^2)/(shadow.a^2) + ((j - shadow.y)^2)/(shadow.b^2);
        if(outcome <= 1)
            roiY = j-(shadow.y-shadow.b);
            roiX = i-(shadow.x-shadow.a);
            
            if(roi.with_shadow(j,i,1) ~= 255 && roi.with_shadow(j,i,2) ~= 255 && roi.with_shadow(j,i,3) ~= 255)
                bg_image_with_shadow(j,i,:) = roi.with_shadow(j,i,:);
            end
        end
    end
end


roi.with_shadow_small = zeros(shadow.b*2, shadow.a*2, 3);
for i = 1:1:bg_imageSize(1,2)
    for j = 1:1:bg_imageSize(1,1)
        outcome = ((i - shadow.x)^2)/(shadow.a^2) + ((j - shadow.y)^2)/(shadow.b^2);
        if(outcome <= 1)
            
            roiY = j-(shadow.y-shadow.b);
            roiX = i-(shadow.x-shadow.a);
            
            if roiY == 0
                roiY = 1;
            end
            
            if roiX == 0
                roiX = 1;
            end
            
            roi.with_shadow_small(roiY,roiX,:) = roi.with_shadow(j,i,:);
        end
    end
end


%% Load Test Insert Images - Tea Pot
%%insert_image = imread('test_insert.png');
insert_image = imread('sample.png');
originImage = imresize(insert_image,1.15);

[wi he de] = size(originImage);
%[wi he de] = size(insert_image);

for i = 1:wi
    for j = 1:he
        for k = 1:de
            if (originImage(i,j,k) > 10)
                bg_image_with_shadow(i+105,j+127,k) = originImage(i,j,k);
            end
        end
    end
end

%% Display Comparsion
figure; hold on;
subplot(2,2,1); colormap('colorcube'); imagesc(bg_image./255), title('Background');
subplot(2,2,2); colormap('gray'); imagesc(roi.original./255), title('ROI');
subplot(2,2,3); imagesc(bg_image_with_shadow./255), title('Adjusted Image');
subplot(2,2,4); imagesc(roi.with_shadow_small./255), title('Adjusted ROI');
