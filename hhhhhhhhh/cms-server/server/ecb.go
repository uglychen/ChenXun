package server

import (
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"fmt"
)

type ecb struct {
	b         cipher.Block
	blockSize int
}

func newECB(b cipher.Block) *ecb {
	return &ecb{
		b:         b,
		blockSize: b.BlockSize(),
	}
}

type ecbEncrypter ecb

// NewECBEncrypter returns a BlockMode which encrypts in electronic code book
// mode, using the given Block.
func NewECBEncrypter(b cipher.Block) cipher.BlockMode {
	return (*ecbEncrypter)(newECB(b))
}

func (x *ecbEncrypter) BlockSize() int { return x.blockSize }

func (x *ecbEncrypter) CryptBlocks(dst, src []byte) {
	if len(src)%x.blockSize != 0 {
		panic("crypto/cipher: input not full blocks")
	}
	if len(dst) < len(src) {
		panic("crypto/cipher: output smaller than input")
	}
	for len(src) > 0 {
		x.b.Encrypt(dst, src[:x.blockSize])
		src = src[x.blockSize:]
		dst = dst[x.blockSize:]
	}
}

type ecbDecrypter ecb

// NewECBDecrypter returns a BlockMode which decrypts in electronic code book
// mode, using the given Block.
func NewECBDecrypter(b cipher.Block) cipher.BlockMode {
	return (*ecbDecrypter)(newECB(b))
}

func (x *ecbDecrypter) BlockSize() int { return x.blockSize }

func (x *ecbDecrypter) CryptBlocks(dst, src []byte) {
	if len(src)%x.blockSize != 0 {
		panic("crypto/cipher: input not full blocks")
	}
	if len(dst) < len(src) {
		panic("crypto/cipher: output smaller than input")
	}
	for len(src) > 0 {
		x.b.Decrypt(dst, src[:x.blockSize])
		src = src[x.blockSize:]
		dst = dst[x.blockSize:]
	}
}


//AES ECB模式的加密解密
type AesTool struct {
	//128 192  256位的其中一个 长度 对应分别是 16 24  32字节长度
	Key       []byte
	BlockSize int
}

func NewAesTool(key []byte, blockSize int) *AesTool {
	return &AesTool{Key: key, BlockSize: blockSize}
}

func (this *AesTool) padding(src []byte) []byte {
	//填充个数
	paddingCount := aes.BlockSize - len(src)%aes.BlockSize
	if paddingCount == 0 {
		return src
	} else {
		//填充数据
		return append(src, bytes.Repeat([]byte{byte(0)}, paddingCount)...)
	}
}

//unpadding
func (this *AesTool) unPadding(src []byte) []byte {
	for i := len(src) - 1; ; i-- {
		if src[i] != 0 {
			return src[:i+1]
		}
	}
	return nil
}

func (this *AesTool) Encrypt(src []byte) ([]byte, error) {
	//key只能是 16 24 32长度
	block, err := aes.NewCipher([]byte(this.Key))
	if err != nil {
		return nil, err
	}
	//padding
	src = this.padding(src)
	//返回加密结果
	encryptData := make([]byte, len(src))
	//存储每次加密的数据
	tmpData := make([]byte, this.BlockSize)

	//分组分块加密
	for index := 0; index < len(src); index += this.BlockSize {
		block.Encrypt(tmpData, src[index:index+this.BlockSize])
		copy(encryptData, tmpData)
	}
	return encryptData, nil
}
func (this *AesTool) Decrypt(src []byte) ([]byte, error) {
	//key只能是 16 24 32长度
	block, err := aes.NewCipher([]byte(this.Key))
	if err != nil {
		return nil, err
	}
	//返回加密结果
	decryptData := make([]byte, len(src))
	//存储每次加密的数据
	tmpData := make([]byte, this.BlockSize)

	//分组分块加密
	for index := 0; index < len(src); index += this.BlockSize {
		block.Decrypt(tmpData, src[index:index+this.BlockSize])
		copy(decryptData, tmpData)
	}
	return this.unPadding(decryptData), nil
}

//测试padding  unpadding
func TestPadding() {
	tool := NewAesTool([]byte{}, 16)
	src := []byte{1, 2, 3, 4, 5}
	src = tool.padding(src)
	fmt.Println(src)
	src = tool.unPadding(src)
	fmt.Println(src)
}

//测试AES ECB 加密解密
func TestEncryptDecrypt() {
	key := []byte{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}
	blickSize := 16
	tool := NewAesTool(key, blickSize)
	encryptData, _ := tool.Encrypt([]byte("32334erew32少时诵诗书所所所所所"))
	fmt.Println(encryptData)
	decryptData, _ := tool.Decrypt(encryptData)
	fmt.Println(string(decryptData))
}
