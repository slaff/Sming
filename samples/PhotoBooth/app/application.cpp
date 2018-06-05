#include <user_config.h>
#include "SmingCore/SmingCore.h"
#include "SmingCore/Network/SmtpClient.h"

#include "Libraries/A6/A6.h"

SmtpClient client;

int onServerError(SmtpClient& client, int code, char* status)
{
	debugf("Status: %s", status);

	return 0; // return non-zero value to abort the connection
}

int onMailSent(SmtpClient& client, int code, char* status)
{
	// get the sent mail message
	MailMessage* mail = client.getCurrentMessage();

	debugf("Mail sent. Status: %s", status);

	// And if there are no more pending emails then you can disconnect from the server
	if(!client.countPending()) {
		debugf("No more mails to send. Quitting...");
		client.quit();
	}

	return 0;
}

void onConnected(IPAddress ip, IPAddress mask, IPAddress gateway)
{
#ifdef ENABLE_SSL
	client.addSslOptions(SSL_SERVER_VERIFY_LATER);
#endif

	// Take a picture and send it as an email
	client.onServerError(onServerError);

	String dsn = "smtp://username:password@host:port" ;
	debugf("Connecting to SMTP server using: %s", dsn.c_str());

	client.connect(URL(dsn));

	MailMessage* mail = new MailMessage();
	mail->from = MAIL_FROM;
	mail->to = MAIL_TO;
	mail->subject = "Picture from the A20 device";
	mail->setBody("Look at the attachments.");

	FileStream* file= new FileStream("image.png");
	mail->addAttachment(stream, MIME_JPEG, "image.jpg");

	client.onMessageSent(onMailSent);
	client.send(mail);
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	/* Toggle UART0 to use pins GPIO13/GPIO15 as RX and TX */
#if 0
	Serial.swap();
#endif

	/**
	 * Serial1 uses UART1, TX pin is GPIO2.
	 * UART1 can not be used to receive data because normally
	 * it's RX pin is occupied for flash chip connection.
	 *
	 * If you have a spare serial to USB converter do the following to see the
	 * messages printed on UART1:
	 * - connect converter GND to esp8266 GND
	 * - connect converter RX to esp8266 GPIO2
	 */
	Serial1.begin(SERIAL_BAUD_RATE);
	Serial1.systemDebugOutput(true); // UART1 will be used to log debug information

	// Setup the WIFI connection
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD); // Put you SSID and Password here

	WifiEvents.onStationGotIP(onConnected);
}
