/*
 * Userspace program that communicates with the vga_ball device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "vga_ball.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int vga_ball_fd;

/* Read and print the background color */
void print_background_color() {
  vga_ball_arg_t vla;
  
  if (ioctl(vga_ball_fd, VGA_BALL_READ_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_READ_BACKGROUND) failed");
      return;
  }
  printf("%02x %02x %02x\n",
	 vla.background.red, vla.background.green, vla.background.blue);
}

/* Set the background color */
void set_background_color(const vga_ball_color_t *c)
{
  vga_ball_arg_t vla;
  vla.background = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
      return;
  }
}

int main()
{
  vga_ball_arg_t vla;
  int i;
  static const char filename[] = "/dev/vga_ball";
  struct ball_position pos = {0, 0}; // 初始位置
  int dx = 2; // 水平移动速度
  int direction = 1; // 移动方向，1为右移，-1为左移
  static const vga_ball_color_t colors[] = {
    { 0xff, 0x00, 0x00 }, /* Red */
    { 0x00, 0xff, 0x00 }, /* Green */
    { 0x00, 0x00, 0xff }, /* Blue */
    { 0xff, 0xff, 0x00 }, /* Yellow */
    { 0x00, 0xff, 0xff }, /* Cyan */
    { 0xff, 0x00, 0xff }, /* Magenta */
    { 0x80, 0x80, 0x80 }, /* Gray */
    { 0x00, 0x00, 0x00 }, /* Black */
    { 0xff, 0xff, 0xff }  /* White */
  };

# define COLORS 9

  printf("VGA ball Userspace program started\n");

  if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  printf("initial state: ");
  print_background_color();

  for (i = 0 ; i < 24 ; i++) {
    set_background_color(&colors[i % COLORS ]);
    print_background_color();
    usleep(400000);
  }
  
  printf("VGA BALL Userspace program terminating\n");

    while (1) {
        pos.x += dx * direction;

        // 边界检查，改变方向
        if (pos.x < 0 || pos.x > 640) { // 假设屏幕宽度为640
            direction *= -1;
            pos.x += dx * direction; // 立即改变方向，避免卡在边界
        }

        // 更新小球位置
        if (ioctl(fd, VGA_BALL_SET_POSITION, &pos) < 0) {
            perror("Failed to set ball position");
            break;
        }

        usleep(20000); // 控制更新速度，20毫秒
    }



  return 0;
}
