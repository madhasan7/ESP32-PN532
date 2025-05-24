#include <PN532_HSU.h>
#include <PN532.h>

#define PN532_RX 16  // RX pada GPIO19
#define PN532_TX 17  // TX pada GPIO18

// Inisialisasi Serial_PN532 sebagai Serial2
HardwareSerial Serial_PN532(2); // 2 untuk Serial2
PN532_HSU pn532hsu(Serial_PN532);
PN532 nfc(pn532hsu);

void setup(void) {
  // Inisialisasi Serial untuk debugging
  Serial.begin(115200);

  // Konfigurasi Serial_PN532 (yang merupakan Serial2) untuk komunikasi dengan PN532
  Serial_PN532.begin(115200, SERIAL_8N1, PN532_RX, PN532_TX);
  Serial.println("Inisialisasi PN532 RFID Reader...");

  // Mulai komunikasi dengan PN532
  nfc.begin();

  // Cek apakah PN532 terdeteksi
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Tidak dapat mendeteksi PN532. Periksa koneksi.");
    while (1); // Hentikan jika tidak terdeteksi
  }

  // Jika terdeteksi, cetak informasi firmware
  Serial.print("Chip PN5 ditemukan: "); 
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Atur SAM (Security Access Module) untuk mengaktifkan fitur pembacaan kartu
  nfc.SAMConfig();

  Serial.println("Siap membaca kartu RFID...");
}

String readRFID() {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer untuk UID
  uint8_t uidLength;                        // Panjang UID (4 atau 7 bytes)
  String uidString = "";                    // String untuk menyimpan UID

  // Tunggu kartu ISO14443A (Mifare, dll.) terdeteksi
bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

  if (success) {
    // Tampilkan UID kartu yang terdeteksi
    for (uint8_t i = 0; i < uidLength; i++) {
      uidString += String(uid[i], HEX);
      if (i < uidLength - 1) {
        uidString += " "; // Tambahkan ' ' antara nilai UID
      }
    }
    Serial.println("Kartu RFID terdeteksi dengan UID: " + uidString);
    uidString.replace(" ", "%20");
//    String response = sendHttpRequest(url_server + uidString, ""); 
//      if (!response.isEmpty()) {
//      processResponse(response);
//    }

    // Tunggu sebelum membaca kartu lagi
delay (2000);
  }

  return uidString; // Kembalikan UID sebagai string
}

void loop(void) {

  readRFID();  
}
