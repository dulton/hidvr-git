

#ifndef __CGI_H__
#define __CGI_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "httpd.h"

extern int CGI_moo(HTTPD_SESSION_t* session);

extern int CGI_system_upgrade(HTTPD_SESSION_t* session);
extern int CGI_system_upgrade_get_rate(HTTPD_SESSION_t* session);

extern int CGI_flv_live_view(HTTPD_SESSION_t* session);
extern int CGI_playback(HTTPD_SESSION_t* session);

extern int CGI_whoami(HTTPD_SESSION_t* session);
extern int CGI_shell(HTTPD_SESSION_t* session);

extern int cgi_capture_a_jpeg(int vin, int width, int height, const char* file_name);
extern int CGI_snapshot(HTTPD_SESSION_t* session);
extern int CGI_sp(HTTPD_SESSION_t* session);
extern int CGI_mjpeg(HTTPD_SESSION_t* session);
extern int CGI_mjpeg_html(HTTPD_SESSION_t* session);

extern int CGI_live_mp4(HTTPD_SESSION_t* http_session);


// server: the smtp server address, default following the user mail address
// port: the smtp server address opening port, default 25
// usermail: which is the sendor
// password: sendor password
// target: which is the receiver
// subject: the email subject, defualt 'no subject'
// content: the email text content, defualt '<null>'
// snapshot : yes/no, whether to get an image from device, default no
// vin : the snapshot camera channel of device
// size : the image size of snapshot
extern int CGI_send_email(HTTPD_SESSION_t* session);


extern int CGI_jscript(HTTPD_SESSION_t* session);
extern int CGI_gw(HTTPD_SESSION_t* session);
extern int CGI_index(HTTPD_SESSION_t *session);
extern int CGI_snapshot2(HTTPD_SESSION_t* session);

#ifdef __cplusplus
};
#endif
#endif //__CGI_H__

