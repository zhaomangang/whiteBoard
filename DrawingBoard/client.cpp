#include "client.h"
#include <drawwidget.h>

Client::Client(int por,QHostAddress *IP,QString Name)
{
    port = por;
    serverIP = IP;
    userName = Name;

}
