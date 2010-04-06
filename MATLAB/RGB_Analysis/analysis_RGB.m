%% White and Black
clear all;close all;clc;
fid = fopen('wall.raw','r');
bg_imageW = fread(fid);
bg_imageShowW = zeros(240,320,3);
for i = 1:240
    for j = 1:320
        for k = 1:3
            index = 1 + 3*(j-1) + (i-1)*320*3 + (k-1);
                bg_imageShowW(i,j,k) = (bg_imageW(index,1));
        end
    end
end
figure(1);
image(bg_imageShowW./255);

rW = reshape(bg_imageShowW(:,:,1), 1, 240*320);
gW = reshape(bg_imageShowW(:,:,2), 1, 240*320);
bW = reshape(bg_imageShowW(:,:,3), 1, 240*320);

figure(2); hold on;
subplot(2,2,1); hist(rW, 0:1:255); title('rWhite');
subplot(2,2,2); hist(gW, 0:1:255); title('gWhite');
subplot(2,2,3); hist(bW, 0:1:255); title('bWhite');

fid = fopen('keyboard.raw','r');
bg_imageB = fread(fid);
bg_imageShowB = zeros(240,320,3);
for i = 1:240
    for j = 1:320
        for k = 1:3
            index = 1 + 3*(j-1) + (i-1)*320*3 + (k-1);
                bg_imageShowB(i,j,k) = (bg_imageB(index,1));
        end
    end
end
figure(3);
image(bg_imageShowB./255);

rB = reshape(bg_imageShowB(:,:,1), 1, 240*320);
gB = reshape(bg_imageShowB(:,:,2), 1, 240*320);
bB = reshape(bg_imageShowB(:,:,3), 1, 240*320);

figure(4); hold on;
subplot(2,2,1); hist(rB, 0:1:255); title('rBlack');
subplot(2,2,2); hist(gB, 0:1:255); title('gBlack');
subplot(2,2,3); hist(bB, 0:1:255); title('bBlack');

%% red and blue target
%%Notice: There will be a green line in the value 255, it is the background
%%color in .png
clear all;close all;clc;
red_image = imread('redTarget.png');
blue_image = imread('blueTarget.png');

figure(5);
hist(red_image(:,:,1), 0:1:255); title('red_r');
figure(6);
hist(red_image(:,:,2), 0:1:255); title('red_g');
figure(7);
hist(red_image(:,:,3), 0:1:255); title('red_b');
figure(8);
hist(blue_image(:,:,1), 0:1:255); title('blue_r');
figure(9);
hist(blue_image(:,:,2), 0:1:255); title('blue_g');
figure(10);
hist(blue_image(:,:,3), 0:1:255); title('blue_b');