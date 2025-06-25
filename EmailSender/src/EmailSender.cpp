#include "EmailSender.h"

EmailSender* staticInstance = nullptr;

EmailSender::EmailSender(const char* ssid,
                         const char* password,
                         const char* smtp_host,
                         uint16_t smtp_port,
                         const char* author_email,
                         const char* author_password,
                         const char* recipient_email)
  : _ssid(ssid), _password(password), _smtpHost(smtp_host), _smtpPort(smtp_port),
    _authorEmail(author_email), _authorPassword(author_password), _recipientEmail(recipient_email) {
  staticInstance = this;
}

void EmailSender::begin() {
  _connectWiFi();
  MailClient.networkReconnect(true);
  _smtp.debug(1);
  _smtp.callback(_smtpStaticCallback);
}

void EmailSender::email_this_alert(const String& subject, const String& body) {
  Session_Config config;
  config.server.host_name = _smtpHost;
  config.server.port = _smtpPort;
  config.login.email = _authorEmail;
  config.login.password = _authorPassword;
  config.login.user_domain = "";
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;

  SMTP_Message message;
  message.sender.name = F("ESP");
  message.sender.email = _authorEmail;
  message.subject = subject;
  message.addRecipient(F("Recipient"), _recipientEmail);
  message.text.content = body.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success |
                            esp_mail_smtp_notify_failure |
                            esp_mail_smtp_notify_delay;

  if (!_smtp.connect(&config)) {
    Serial.printf("Connection error: %s\n", _smtp.errorReason().c_str());
    return;
  }

  if (!MailClient.sendMail(&_smtp, &message)) {
    Serial.printf("Send error: %s\n", _smtp.errorReason().c_str());
  }
}

void EmailSender::_connectWiFi() {
  WiFi.begin(_ssid, _password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void EmailSender::_smtpStaticCallback(SMTP_Status status) {
  if (staticInstance) {
    staticInstance->_smtpCallback(status);
  }
}

void EmailSender::_smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
  if (status.success()) {
    for (size_t i = 0; i < _smtp.sendingResult.size(); i++) {
      SMTP_Result result = _smtp.sendingResult.getItem(i);
      Serial.printf("Sent to: %s | Subject: %s\n",
                    result.recipients.c_str(), result.subject.c_str());
    }
    _smtp.sendingResult.clear();
  }
}
