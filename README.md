# youtube-share-code-for-public-
Write your own handler function (driver code).
# Tự viết hàm xử lý ngắt của chính mình mà không cần sử dụng bất cứ một phụ thuộc nào từ hẵng 
# bằng cách di chuyển con trỏ vector table lên vị trí của RAM ta có thể tự viết hàm xử lý ngắt của chính mình và đặt lại tên hàm xử lý ngắt 
```c
char *vector_table = 0x00;
char *ram = 0x20000000; // refer doccument 

for(int i = 0; i < 0x194; i++)
{
    ram[i] = vector_table[i];
}

uint32_t *VTOR = (uint32_t *)0xE000ED08;
*VTOR = 0x20000000;

uint32_t *ptr = (uint32_t*)0x20000058;
*ptr = (uint32_t)my_handler;

```

# Không còn lệ thuộc hoàn toàn kiểm soát mọi thứ 
# kết hợp với USART Trong hàm xử lý ngắt.

