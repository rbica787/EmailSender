#ifndef EMAIL_SENDER_H
#define EMAIL_SENDER_H

#include <ESP_Mail_Client.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

class EmailSender {
  public:
    EmailSender(const char* ssid,
                const char* password,
                const char* smtp_host,
                uint16_t smtp_port,
                const char* author_email,
                const char* author_password,
                const char* recipient_email);

    void begin();
    void email_this_alert(const String& subject, const String& body);

  private:
    const char* _ssid;
    const char* _password;
    const char* _smtpHost;
    uint16_t _smtpPort;
    const char* _authorEmail;
    const char* _authorPassword;
    const char* _recipientEmail;

    SMTPSession _smtp;
    void _connectWiFi();
    void _smtpCallback(SMTP_Status status);
    static void _smtpStaticCallback(SMTP_Status status);
};

#endif
