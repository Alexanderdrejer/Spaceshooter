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

  // Struct to get event-based access to GPIO line
  struct gpioevent_request req[6];
  // Structure to store values read from GPIO lines
  struct gpioevent_data data[6];

  // Fill struct elements with 0's to clear old values
  for (int i = 0; i < 6; ++i)
    memset(&req[i], 0, sizeof(req[i]));

  // Specify GPIO lines we want to access
  req[0].lineoffset = BTN_A;
  req[1].lineoffset = BTN_B;
  req[2].lineoffset = BTN_UP;
  req[3].lineoffset = BTN_DOWN;
  req[4].lineoffset = BTN_LEFT;
  req[5].lineoffset = BTN_RIGHT;

  for (int i = 0; i < 6; ++i) {
    // Lines are set as inputs
    req[i].handleflags = GPIOHANDLE_REQUEST_INPUT;
    // Lines report both rising and falling edges
    req[i].eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
    // Set a name to be shown when gpioinfo is called
    strcpy(req[i].consumer_label, "GPIO event monitor");

    // Use ioctl() to get GPIO line event from kernel
    int ret_val = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req[i]);
    if (ret_val < 0) {
      perror("ioctl()");
      for (int j = 0; j < 6; ++j)
        close(req[j].fd);
      close(fd);
      return EXIT_FAILURE;
    }
  }

  while (true) {
    for (int i = 0; i < 6; ++i) {
      // Use read() to get GPIO line value from kernel
      ssize_t bytes = read(req[i].fd, &data[i], sizeof(data[i]));
      // std::cout << "Waiting...\n";
      if (bytes != sizeof(data[i])) {
        perror("read()");
      }
      else {
	// If rising edge was detected
        if (data[i].id == GPIOEVENT_EVENT_RISING_EDGE) {
          std::cout << "Button " << i << ": rising edge\n";
        }
	// If falling edge was detected
        if (data[i].id == GPIOEVENT_EVENT_FALLING_EDGE) {
          std::cout << "Button " << i << ": falling edge\n";
        }
      }
    }
  }

  for (int i = 0; i < 6; ++i) {
    close(req[i].fd);
  }
  close(fd);
  return EXIT_SUCCESS;
}
