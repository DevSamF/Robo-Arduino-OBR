void inclinacaoX (){
  //**************************************************************************************************************************
  // Começa uma transmissão com o sensor
  Wire.beginTransmission(ENDERECO_SENSOR);
  // Enfilera os bytes a serem transmitidos para o sensor
  // Começando com o registro 0x3B
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  // Finaliza e transmite os dados para o sensor. O false fará com que seja enviado uma mensagem
  // de restart e o barramento não será liberado
  Wire.endTransmission(false);
  // Solicita os dados do sensor, solicitando 14 bytes, o true fará com que o barramento seja liberado após a solicitação
  // (o valor padrão deste parâmetro é true)
  Wire.requestFrom(ENDERECO_SENSOR, 14, true);
  // Armazena o valor dos sensores nas variáveis correspondentes
  acelX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  // Armazena a amostra mais recente no array
  amostrasX[indice] = acelX;
  // Calcula a média móvel
  int mediaX = 0;
  for (int i = 0; i < numAmostras; i++) {
    mediaX += amostrasX[i];
  }
  mediaX /= numAmostras;
  // Atualiza o índice para a próxima amostra
  indice = (indice + 1) % numAmostras;
  // Printa o valor X do acelerômetro na serial
  Serial.print("Acelerômetro X = ");
  Serial.println(mediaX);
if (mediaX >= 1000) {
  ladeira = 2;
} else {
  ladeira = 0;
}
}