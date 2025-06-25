#include <EmailSender.h>

EmailSender email(
  "YourSSID",
  "YourPassword",
  "smtp.gmail.com",
  465,
  "you@gmail.com",
  "your_app_password",
  "target@email.com"
);

void setup() {
  Serial.begin(115200);
  email.begin();
  email.email_this_alert("Test Subject", "This is the email body");
}

void loop() {}
