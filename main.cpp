#include "./gtts.h"
//#include <syslog.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gprintf.h>
#include <memory>
#include <axsdk/axhttp.h>

//#define LOG(fmt, args...)    { syslog(LOG_INFO, fmt, ## args); printf(fmt, ## args); }
//#define LOG_ERROR(fmt, args...)    { syslog(LOG_CRIT, fmt, ## args); printf(fmt, ## args); }

AXHttpHandler  *httpHandler = 0;
typedef std::string string;

static void
httpCallback( const gchar *path, const gchar *method, const gchar *query, GHashTable *params, GOutputStream *stream, gpointer user_data) {
  const char *valuestring, *langstring;
  GDataOutputStream *http = g_data_output_stream_new(stream);;
  guint value;

  LOG("method: %s, query= %s\n", method, query );


  //Send HTTP header to JSON data response
  g_data_output_stream_put_string( http, "Content-Type: application/json; charset=utf-8\r\n\r\n", NULL, NULL);
  
  valuestring = (const char*)g_hash_table_lookup(params,"trans");
  langstring = (const char*)g_hash_table_lookup(params,"lan");
  if( !valuestring ) {
    g_data_output_stream_put_string( http, "{\"error\":\"Missing option\"}", NULL, NULL);
    LOG("Event was not fired as no option or value value was defined.  Syntax = /events?fire=[0,1]\n");
      g_object_unref(http);
    return;
  }
  
  string msg(valuestring);
  string lang(langstring);
  std::unique_ptr<gtts::GoogleTTS> tts(new gtts::GoogleTTS(msg, lang));
  tts->execute();

  
  g_object_unref(http);
}


int
main(void) {
  GMainLoop *loop;
  openlog( PACKAGE_NAME, LOG_PID | LOG_CONS, LOG_USER);
  loop = g_main_loop_new( NULL, FALSE) ;

  //eventHandler = ax_event_handler_new();

  httpHandler = ax_http_handler_new( httpCallback, NULL);
  
  g_main_loop_run( loop );
  LOG("complete the application %s\n", PACKAGE_NAME);
  //No need to clean up as ACAP platform will do this when application exits
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
