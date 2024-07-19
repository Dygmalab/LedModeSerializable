This repository holds the LED abstractions for the effects reproduced on the keyscanner, the following code has snippets form the FastLED project for certain effects, https://github.com/FastLED/FastLED

## Mapper LED EFFECT 
The mapper LED effect is a simple effect that maps the key scanner to the LED strip
To use this effect you need to uncomment the following line in the `pinout.h` file
```cpp
#define LED_MAPPING_CREATION
```
Once you have done that, this LED effect will replace the disconnect effect, you will see
the LED strip light up in a sequence from the first key to the last key, and then turn off.

### How to use it

Every LED has a unique position on the matrix, this position will be stored and printed on the serial monitor,
to use this effect you need to know the LED you want to light up, If the LED you have to press any key and the 
position will be stored in the position array, and printed on the serial monitor.

Example of the serial monitor output:
![image_480.png](..%2F..%2Fdoc%2Freadme_images%2Fimage_480.png)

You will have to repeat this process for every LED you want to light up, once you have all the positions you want to light up,
you can declare the new LED map on the pinout.h file, as follows:
```cpp
    const  uint8_t led_mapping_right[] = {
            0, 4, 8, 12, 16, 20, 24,
            1, 5, 9, 13, 17, 21, 25, 27,
            2, 6, 10, 14, 18,22, 26,
            28, 32, 29, 33, 30, 34,
            3, 7, 11, 15, 19, 23,
            31, 35
    };
```
The LED mapping is an array that maps the key scanner to the LED strip
the first element of the array is the first key of the key scanner,
and the last element is the last key of the key scanner.

Once you have finished you must comment the following line in the `pinout.h` file
```cpp
#define LED_MAPPING_CREATION
```
### Testing

To test the LED mapping, you can use the Stalker LED effect; this effect will light up the LED in the pressed key.
If there was an error in the mapping, the LED will not light up in the correct position, and you will have to repeat the process.