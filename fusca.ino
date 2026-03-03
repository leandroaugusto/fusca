// ================= PINOS =================
#define LANT_FRENTE   2
#define LANT_TRAS     3
#define PISCA_ESQ     5
#define PISCA_DIR     6

#define SW_LANTERNA   7
#define SW_PISCA_ESQ  8
#define SW_PISCA_DIR  9

// ============== CONFIG ===================
unsigned long tempoPisca = 500; // ms
unsigned long ultimoPisca = 0;
bool estadoPisca = false;

void setup() {
  // Saídas
  pinMode(LANT_FRENTE, OUTPUT);
  pinMode(LANT_TRAS, OUTPUT);
  pinMode(PISCA_ESQ, OUTPUT);
  pinMode(PISCA_DIR, OUTPUT);

  // Entradas (interruptores)
  pinMode(SW_LANTERNA, INPUT_PULLUP);
  pinMode(SW_PISCA_ESQ, INPUT_PULLUP);
  pinMode(SW_PISCA_DIR, INPUT_PULLUP);
}

void loop() {
  // ===== LANTERNAS =====
  bool lanternaLigada = (digitalRead(SW_LANTERNA) == LOW);

  digitalWrite(LANT_FRENTE, lanternaLigada);
  digitalWrite(LANT_TRAS,   lanternaLigada);

  // ===== CONTROLE DO TEMPO DO PISCA =====
  if (millis() - ultimoPisca >= tempoPisca) {
    ultimoPisca = millis();
    estadoPisca = !estadoPisca;
  }

  // ===== PISCA ESQUERDO =====
  if (digitalRead(SW_PISCA_ESQ) == LOW) {
    digitalWrite(PISCA_ESQ, estadoPisca);
  } else {
    digitalWrite(PISCA_ESQ, LOW);
  }

  // ===== PISCA DIREITO =====
  if (digitalRead(SW_PISCA_DIR) == LOW) {
    digitalWrite(PISCA_DIR, estadoPisca);
  } else {
    digitalWrite(PISCA_DIR, LOW);
  }
}
