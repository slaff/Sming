Control your Panasonic Viera TV remotely from Sming
===================================================

.. highlight:: c++

Demonstrates remote control of a Panasonic Viera TV from a Sming application.

Using
-----

1. Add ``COMPONENT_DEPENDS += Panasonic-Viera`` to your application componenent.mk file.
2. Add these lines to your application::

   #include <Panasonic/VieraTV/Client.h>

   using namespace Panasonic;

   VieraTV::Client client;

   void onConnected(HttpConnection& connection, XML::Document& description)
   {
      auto node = XML::getNode(description, F("/device/friendlyName"));
      Serial.println(_F("New Viera TV found."));
      if(node != nullptr) {
         Serial.print(_F("Friendly name: "));
         Serial.println(node->value());
      }

      //...

      client.sendCommand(VieraTV::CommandAction::ACTION_CH_UP);

      client.launch(VieraTV::ApplicationId::APP_YOUTUBE);
   }

   // Call when IP address has been obtained
   void onIp(IpAddress ip, IpAddress mask, IpAddress gateway)
   {
      // ...

      if(!UPnP::deviceHost.begin()) {
         debug_e("UPnP initialisation failed");
         return;
      }

      /* The command below will use UPnP to auto-discover a Panasonic Viera TV */
      client.connect(onConnected);

      // ...
   }

See the sample application for details.