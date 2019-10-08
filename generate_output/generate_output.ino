int gen_number(){
  return random(100);
}

int* gen_array(int len){
  int *arr = (int *)malloc(len * 4);
  for(int i=0; i<len; i++){
    arr[i] = gen_number();
  }
  return arr;
}

void print_array(int *array_p, int len){
  for(int i=0; i<len-1; i++){
    Serial.print(array_p[i]);
    Serial.print(",");
  }
  Serial.print(array_p[len-1]);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Test Generate Data");

  
}

void loop() {
  // put your main code here, to run repeatedly:
  int length = 5;
  for(int i=0; i<100; i++){
    int* a = gen_array(5);
    print_array(a, 5);
    free(a);
//    Serial.print(a);
    Serial.print("\n");
  }
}
