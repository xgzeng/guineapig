#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <string.h>

// generate keypair with
///  openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:1024
// export pubkey with
///  openssl rsa -pubout -in private_key.pem -out public_key.pem

int main(int argc, char* argv[]) {
  RSA* rsa = 0;

  // read rsa pub key from public_key.pem
  // 读取公密钥文件
  {
    FILE* key_file = fopen("public_key.pem", "rb");
    if (key_file == 0) {
        printf("Open public_key.pem error\n");
        return 1;
    }
    rsa = PEM_read_RSA_PUBKEY(key_file, &rsa, 0, 0);
  }

  // 读取私钥文件
  {
    FILE* key_file = fopen("private_key.pem", "rb");
    if (!key_file) {
      printf("Open private_key.pem error\n");
      return 1;
    }
    
    rsa = PEM_read_RSAPrivateKey(key_file, &rsa, 0, 0);
    if (rsa == 0) {
        printf("read private_key.pem error\n");
        return 1;
    }
  }

  // 将公钥转换为2进制字节
  // 结果应为:
  //  BFA99EEB367E7FF6CE7ECA380D6ED02F2D90EA69F6447207516271C2B9F3B60AD67E8
  //  2926F0927320AA87D379BD9C02F634D29422804D25FD84BAFE2F75709E400DA305DEE
  //  96DDB10F36FE222161091769A1543F469F169E2C6FBCCE6540BA3897B5300ACED977D
  //  D807F324F6307CF70B4B0F019781AF1F1949B3A194860B651
  int key_size = BN_num_bytes(rsa->n);
  printf("ras key size is %d bytes\n", key_size);

  unsigned char key_bytes[512];
  int n = BN_bn2bin(rsa->n, key_bytes);
  printf("convert to %d bytes: %X %X ... %X %X\n",
          n, key_bytes[0], key_bytes[1],
          key_bytes[n - 2], key_bytes[n - 1]);

  printf("rsa key: %s\n", BN_bn2hex(rsa->n));

  // 使用公钥加密
  char clear_txt[] = "hello world";
  unsigned char crypt_txt[256];

  n = RSA_public_encrypt(strlen(clear_txt),
        reinterpret_cast<unsigned char*>(clear_txt),
        crypt_txt,
        rsa, RSA_PKCS1_OAEP_PADDING);

  printf("crypt text(%d bytes): %X %X ... %X %X\n", n,
         crypt_txt[0], crypt_txt[1],
         crypt_txt[n - 2], crypt_txt[n - 1]
  );

  unsigned char decrypted_txt[256];
  n = RSA_private_decrypt(n, crypt_txt, decrypted_txt, rsa, RSA_PKCS1_OAEP_PADDING);
  decrypted_txt[n] = 0;
  printf("decrypted text(%d bytes): %s\n", n,
         decrypted_txt);
  
  return 0;
}
