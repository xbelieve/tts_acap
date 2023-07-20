#include "./gtts.h"
//#include <syslog.h>
#include <axsdk/axhttp.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>

//#define LOG(fmt, args...)    { syslog(LOG_INFO, fmt, ## args); printf(fmt, ##
//args); } #define LOG_ERROR(fmt, args...)    { syslog(LOG_CRIT, fmt, ## args);
//printf(fmt, ## args); }
static gchar *glob_app_name = NULL;
AXHttpHandler *httpHandler = 0;
typedef std::string string;

std::string urlDecode(const std::string& url) {
    std::ostringstream decoded;
    decoded.fill('0');

    for (size_t i = 0; i < url.length(); ++i) {
        char c = url[i];
        if (c == '%') {
            if (i + 2 < url.length()) {
                std::string hex = url.substr(i + 1, 2);
                std::istringstream iss(hex);
                int code;
                iss >> std::hex >> code;
                decoded << static_cast<char>(code);
                i += 2;
            }
        } else if (c == '+') {
            decoded << ' ';
        } else {
            decoded << c;
        }
    }

    return decoded.str();
}

static void httpCallback(const gchar *path, const gchar *method,
                         const gchar *query, GHashTable *params,
                         GOutputStream *stream, gpointer user_data) {
  const char *valuestring, *langstring;
  GDataOutputStream *http = g_data_output_stream_new(stream);
  guint value;

  LOG_ERROR("method: %s, query= %s\n", method, query);

  // Send HTTP header to JSON data response
  g_data_output_stream_put_string(
      http, "Content-Type: application/json; charset=utf-8\r\n\r\n", NULL,
      NULL);

  valuestring = (const char *)g_hash_table_lookup(params, "text");
  langstring = (const char *)g_hash_table_lookup(params, "language");
  if (!valuestring) {
    g_data_output_stream_put_string(http, "{\"error\":\"Missing option\"}",
                                    NULL, NULL);
    LOG("Text is not provided, and the syntax is: "
        "http=<ip>/local/tts_go/tts.cgi?text=こんにちは&language=ja\n");
    g_object_unref(http);
    return;
  }

  string msg(urlDecode(valuestring));
  string lang(langstring);
  std::unique_ptr<gtts::GoogleTTS> tts(new gtts::GoogleTTS(msg, lang));
  tts->execute();

  g_object_unref(http);
}

int main(int argc, char *argv[]) {
  GMainLoop *loop;
  glob_app_name = g_path_get_basename(argv[0]);
  openlog(glob_app_name, LOG_PID | LOG_CONS, LOG_USER);
  // openlog( PACKAGE_NAME, LOG_PID | LOG_CONS, LOG_USER);
  loop = g_main_loop_new(NULL, FALSE);

  // eventHandler = ax_event_handler_new();

  httpHandler = ax_http_handler_new(httpCallback, NULL);

  g_main_loop_run(loop);
  LOG("complete the application %s\n", PACKAGE_NAME);
  // No need to clean up as ACAP platform will do this when application exits
  return 0;
}

/*
int main(int argc, char *argv[]) {
    typedef std::string string;
    switch(argc) {
        case 2:
            if (string("-h") == argv[1]
                || string("-H") == argv[1]
                || string("--help") == argv[1]) {
                gtts::GoogleTTS::help();
            }
            if (string("-v") == argv[1] || string("--version") == argv[1]) {
                gtts::GoogleTTS::version();
            }
            if (string("-l") == argv[1] || string("--languages") == argv[1]) {
                gtts::GoogleTTS::languages();
            }
            break;

        case 3: {
            string msg(argv[2]);
            string lang(argv[1]);
            std::unique_ptr<gtts::GoogleTTS>
                tts(new gtts::GoogleTTS(msg, lang));
            tts->execute();
            }
            break;

        case 4: {
            string msg(argv[2]);
            string lang(argv[1]);
            string speed(argv[3]);
            std::unique_ptr<gtts::GoogleTTS>
            tts(new gtts::GoogleTTS(msg, lang, speed));
            tts->execute();
            }
            break;

        default:
            gtts::GoogleTTS::help();
            break;
    }
    return 0;
}*/
