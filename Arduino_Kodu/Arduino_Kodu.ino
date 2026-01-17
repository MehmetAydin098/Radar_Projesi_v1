#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


LiquidCrystal_I2C lcd(0x20, 16, 2);  // Ekran adresimiz
Servo radarServo;


const int servoPin  = 6;
const int buzzerPin = 7;
const int echoPin   = 8;
const int trigPin   = 12;
const int redLed    = 4;  // Tehlike LED'imiz
const int greenLed  = 3;  // Güvenli LED'imiz


int alarmMesafesi = 20; 

long sure;
int mesafe;

void setup() {
  // Pin Modlarımız
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  // Başlangıç Durumu
  digitalWrite(greenLed, HIGH); // Sistemi açtık yeşil yaktık
  
  // Servo motorumuzu ve LCD'yi Başlatma Kodlarımız
  radarServo.attach(servoPin);
  lcd.init();
  lcd.backlight();
  
  // Açılış Ekranı
  lcd.setCursor(0,0);
  lcd.print("Radar Sistemi");
  lcd.setCursor(0,1);
  lcd.print("Baslatiliyor...");
  delay(2000);
  lcd.clear();
}

void loop() {
  
  for (int i = 0; i <= 180; i+=2) { // 2'şer derece atlayarak daha hızlı taramamızı yapsın 
    radarServo.write(i);
    delay(75); // Motorun hedefe varması için bekleme süresi                   NOT:(Videoda bu süre 113ms alınmıştır orda dönüş daha net görünmesi açısından )
    
    mesafe = mesafeOlc(); // Ölçümümüzü yap
    ekranaYaz(i, mesafe); // LCD'ye yaz
    alarmKontrol(mesafe); // Tehlike var mı?
  }
  
  
  for (int i = 180; i >= 0; i-=2) {
    radarServo.write(i);
    delay(75);
    
    mesafe = mesafeOlc();
    ekranaYaz(i, mesafe);
    alarmKontrol(mesafe);
  }
}


int mesafeOlc() {
  digitalWrite(trigPin, LOW); // trig pin'den sinyal (ses dalgası) atma 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // trig pin'den sinyal (ses dalgası) at
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); // tekrar kapat trig pini sinyal gönderme 
  
  sure = pulseIn(echoPin, HIGH);
  int dist = sure * 0.034 / 2;
  return dist;
}


void ekranaYaz(int aci, int dist) {
  // Üst Satır: Açı Bilgisi
  lcd.setCursor(0, 0); 
  lcd.print("Aci: ");
  lcd.print(aci);
  lcd.print(" d   "); // Eski sayıları silmek için boşluklar
  
  // Alt Satır: Mesafe Bilgisi
  lcd.setCursor(0, 1);
  lcd.print("Mesafe: ");
  if(dist > 400 || dist == 0) {
    lcd.print("YOK   "); // Menzil dışı
  } else {
    lcd.print(dist);
    lcd.print(" cm  "); // Eski sayıları silmek için boşluklar
  }
}


void alarmKontrol(int dist) {
  if (dist > 0 && dist < alarmMesafesi) {
    // TEHLİKE DURUMU
    digitalWrite(buzzerPin, HIGH); // Ses ver
    digitalWrite(redLed, HIGH);    // Kırmızı Yak
    digitalWrite(greenLed, LOW);   // Yeşili Söndür
  } else {
    // GÜVENLİ DURUM
    digitalWrite(buzzerPin, LOW);  
    digitalWrite(redLed, LOW);     
    digitalWrite(greenLed, HIGH);  
  }
 
}