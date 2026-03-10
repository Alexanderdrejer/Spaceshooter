#include <linux/gpio.h> // GPIO character device API
#include <sys/ioctl.h> // ioctl() defined here
#include <string.h> // memset() and strlen() defined here
#include <fcntl.h> // open() and O_RDONLY defined here
#include <unistd.h> // close() defined here
#include <iostream>

// Define GPIO lines
#define BTN_A 27
#define BTN_B 22
#define BTN_UP 12
#define BTN_DOWN 16
#define BTN_LEFT 5
#define BTN_RIGHT 6

int main(int argc, char* argv[]) {
  // Open gpiochip0 as read-only device
  int fd = open("/dev/gpiochip0", O_RDONLY);
  if (fd < 0) {
    perror("open()");
    return EXIT_FAILURE;
  }

  // Struct to get access to GPIO line
  struct gpiohandle_request req;
  // Structure to store values read from GPIO lines
  struct gpiohandle_data data;

  // Fill struct elements with 0's to clear old values
  memset(&req, 0, sizeof(req));

  // Specify GPIO lines we want to access
  req.lineoffsets[0] = BTN_A;
  req.lineoffsets[1] = BTN_B;
  req.lineoffsets[2] = BTN_UP;
  req.lineoffsets[3] = BTN_DOWN;
  req.lineoffsets[4] = BTN_LEFT;
  req.lineoffsets[5] = BTN_RIGHT;

  // Access all lines as inputs
  req.flags = GPIOHANDLE_REQUEST_INPUT;
  // Number of gpio lines to access
  req.lines = 6;
  // Set a name to be shown when gpioinfo is called
  strcpy(req.consumer_label, "GPIO read example");

  // Use ioctl() to get GPIO line handle from kernel
  int ret_val = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
  if (ret_val < 0) {
    perror("ioctl()");
    close(req.fd);
    close(fd);
    return EXIT_FAILURE;
  }

  while (true) {
    // Use ioctl() to get GPIO line value from kernel
    ret_val = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
    // std::cout << "Waiting...\n";
    if (ret_val < 0) {
      perror("ioctl");
      close(req.fd);
      close(fd);
      return EXIT_FAILURE;
    }

    for (int i = 0; i < (int)req.lines; ++i)
      if (data.values[i] == 0) { std::cout << "Button " << i << " pressed\n"; }
  }

  close(req.fd);
  close(fd);
  return EXIT_SUCCESS;
}
