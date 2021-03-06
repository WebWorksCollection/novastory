#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

//#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
//#define SMTP_USER "uruchie.org@gmail.com"
//#define SMTP_PASSWORD "uruchiesrver"

#define UPLOAD_LIMIT_BYTES 1 * 1024 * 1024
#define CONTENT_LIMIT_BYTES 5 * 1024 * 1024

#define CACHE_SIZE 35 * 1024 * 1024

#ifdef QT_DEBUG
#define WORKERS_NUMBER 5
#else
#define WORKERS_NUMBER 50
#endif
#define WORKERS_MAX_TIME 180
#ifdef QT_DEBUG
#define WORKER_TIME_LIMIT 60
#else
#define WORKER_TIME_LIMIT 30
#endif
#define WORKER_DISCONNECT_TIMEOUT 5000

#define REPORTS_MAIL "degitx@gmail.com"

#endif // WEBSERVER_CONFIG_H
