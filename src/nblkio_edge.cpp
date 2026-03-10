#include <poll.h> // poll() defined here
#include <linux/gpio.h> // GPIO character device API
#include <sys/ioctl.h> // ioctl() defined here
#include <string.h> // memset() and strlen() defined here
#include <fcntl.h> // open() and O_RDONLY defined here
#include <unistd.h> // close() defined here
#include <iostream>
#include <thread>

// Define GPIO lines
#define BTN_A 27
#define BTN_B 22
#define BTN_UP 12
#define BTN_DOWN 16
#define BTN_LEFT 5
#define BTN_RIGHT 6

#define NUM_BUTTONS 6
#define DEBOUNCE_MS 50

// struct gpioevent_request {
//     __u32 lineoffset;         // GPIO pin-nummer
//     __u32 handleflags;        // INPUT / OUTPUT
//     __u32 eventflags;         // RISING / FALLING / BOTH edges
//     char  consumer_label[32]; // navn vist i gpioinfo
//     int   fd;                 // kernen skriver pin-FD hertil efter ioctl
// };

// struct pollfd {
//     int   fd;      // FD der skal overvåges (kopieret fra req.fd)
//     short events;  // hvad du vil overvåge (POLLIN = data klar)
//     short revents; // hvad kernen rapporterer tilbage efter poll()
// };

struct GPIOState {
    int fd;
    struct pollfd fds[NUM_BUTTONS]; // Tjek kommentaren ovenover for at se hvordan det ser ud!
    struct gpioevent_request req[NUM_BUTTONS];// Tjek kommentaren ovenover for at se hvordan det ser ud!
};

int init(GPIOState &state) {
  state.fd = open("/dev/gpiochip0", O_RDONLY);
    // Kære kernel: Må jeg får en filedescriptor til 
    // din GPIO port, så jeg kan overvåge knap-events? Tak :-)))
    // Her får jeg altså en FD direkte til GPIO-chippen.
  if (state.fd < 0) { perror("open()"); return EXIT_FAILURE; }

  const int pins[NUM_BUTTONS] = { BTN_A, BTN_B, BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT };

  for (int i = 0; i < NUM_BUTTONS; ++i) {
    memset(&state.req[i], 0, sizeof(state.req[i]));
    state.req[i].lineoffset  = pins[i];
    state.req[i].eventflags  = GPIOEVENT_REQUEST_BOTH_EDGES;
    state.req[i].handleflags = GPIOHANDLE_REQUEST_INPUT;
    strcpy(state.req[i].consumer_label, "GPIO event monitor");

    int ret = ioctl(state.fd, GPIO_GET_LINEEVENT_IOCTL, &state.req[i]);
    if (ret < 0) {
      perror("ioctl()");
      for (int j = 0; j < i; ++j) { close(state.req[j].fd); } // kun lukkede FD'er
      close(state.fd);
      return EXIT_FAILURE;
    }

    state.fds[i].fd     = state.req[i].fd;
        // Præcis den pin som kernen har givet mig som jeg kan overvåge.
    state.fds[i].events = POLLIN;
        // Når der er data klar, lad mig vide det. 
          // Når en knap trykkes, lægger GPIO en gpioevent_data-objekt i buffereen
          // poll() vækker tråden da der nu er data.
    std::cout << "Monitoring button " << i << " for edge events...\n";
  }

  return EXIT_SUCCESS;
}

void read_button(GPIOState &state, int idx) {
  struct gpioevent_data data;
    // Structen der udfyldes og lægges i bufferen nåret knap-event er klar
    // og den der bliver læst af read().
      // ser således ud:
        // struct gpioevent_data {
      //     __u64 timestamp; // tidsstempel i nanosekunder
      //     __u32 id;        // GPIOEVENT_EVENT_RISING_EDGE eller GPIOEVENT_EVENT_FALLING_EDGE
      // };

  while (true) {
    state.fds[idx].revents = 0;
            // resetter så jeg ved en fejl ikke behandler et gammelt event.
    int ret = poll(&state.fds[idx], 1, -1);
            // Blokerer indtil knappen trykkes. Når knappet bliver trykket, vil 
            // tråden vågne, da der er data klar på bufferen.
            // VIGTIGT: poll med -1 registreres det af kernen at tråden kun 
            // vågner ved knap-events. Først da, returneres poll. 
            // Jeg undrede mig: Inden CV eller predicator? Det er brugerspace-mekanisme.
              // Vi arbejder i kernelspace!
    if (ret < 0) { perror("poll()"); return; }

    ssize_t bytes = read(state.fds[idx].fd, &data, sizeof(data));
        // Den aflæser eventet og fylder bytes op. 
          // har lagt den ind som fejltjek. Den sikrer, at hele 
          //eventet bliver aflæst og altså hele gpioevent_data udfyldes.
    if (bytes != sizeof(data)) { perror("read()"); continue; }
          // Housekeeping.

    if (data.id == GPIOEVENT_EVENT_RISING_EDGE)
      std::cout << "\nButton " << idx << ": rising edge";
    else if (data.id == GPIOEVENT_EVENT_FALLING_EDGE)
      std::cout << "\nButton " << idx << ": falling edge";

    usleep(DEBOUNCE_MS * 1000); 
      // debounce: ignorer events i 50 ms

    struct gpioevent_data discard;
    while (poll(&state.fds[idx], 1, 0) > 0)
            // Hvis der registreres flere tryk af gangen, fjernes de.
      read(state.fds[idx].fd, &discard, sizeof(discard));
  }

}

int main() {
  GPIOState state;
if (init(state) != EXIT_SUCCESS) return EXIT_FAILURE;

  std::thread threads[NUM_BUTTONS];
  for (int i = 0; i < NUM_BUTTONS; ++i)
    threads[i] = std::thread(read_button, std::ref(state), i);
          // std::ref sender bare state med som reference fremfor kopi.

  for (int i = 0; i < NUM_BUTTONS; ++i)
    threads[i].join();
        // I dette tilfælde bruge rjeg housekeeping her.
        // Trådene stopper reelt aldrig, en hvis en tråd returnerer fejl,.
        // vil join her fange det. Udne join kastes en exception og programmet crasher, RIP.

  for (int i = 0; i < NUM_BUTTONS; ++i) close(state.req[i].fd);
  close(state.fd);
  return EXIT_SUCCESS;
}
