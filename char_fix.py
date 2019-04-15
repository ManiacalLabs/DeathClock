_characters = [
    0b00000000,
    0b00000000,
    0b01010000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01000000,
    0b00110011,
    0b01100110,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00001000,
    0b00000000,
    0b00000000,
    0b01110111,
    0b01000100,
    0b01101011,
    0b01101110,
    0b01011100,
    0b00111110,
    0b00111111,
    0b01100100,
    0b01111111,
    0b01111110,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111101,
    0b01111111,
    0b00110011,
    0b01110111,
    0b00111011,
    0b00111001,
    0b00110111,
    0b01011101,
    0b01000100,
    0b01000111,
    0b01011101,
    0b00010011,
    0b00101101,
    0b00001101,
    0b01110111,
    0b01111001,
    0b01111100,
    0b01111101,
    0b00111110,
    0b00110001,
    0b01010111,
    0b01011001,
    0b01011111,
    0b01001001,
    0b01011110,
    0b01101011,
    0b00110011,
    0b00000000,
    0b01100110,
    0b00000000,
    0b00000010,
    0b00010000,
    0b01101111,
    0b00011111,
    0b00001011,
    0b01001111,
    0b01111011,
    0b00111001,
    0b01111110,
    0b00011101,
    0b00000100,
    0b01000110,
    0b00011101,
    0b00010001,
    0b00101101,
    0b00001101,
    0b00001111,
    0b01111001,
    0b01111100,
    0b00001001,
    0b00111110,
    0b00011011,
    0b00000111,
    0b01011000,
    0b01011010,
    0b01001001,
    0b01011100,
    0b01101011
]

CHAR_OFFSET = 32

# Neosegment modules have LEDs in the following order (the number signifies index):
#     55555
# 4        6
# 4        6
# 4        6
#     33333
# 0        2
# 0        2
# 0        2
#     11111

# /* Segment Layout
# *   11111
# *  2     0
# *  2     0
# *   33333
# *  6     4
# *  6     4
# *   55555
# */

neo_order = [5, 4, 6, 3, 0, 2, 1]
order = [1, 2, 0, 3, 6, 4, 5]


def transform(c):
    res = 0
    for i in range(7):
        if (c & (1 << neo_order[i])):
            res |= (1 << order[i])
    print('{} -> {}'.format(str(c), str(res)))
    return res

def seg(i, c, l):
    return c if l[i] else ' '

new_chars = []
count = 0
for c in _characters:
    c = transform(c)
    new_chars.append(c)
    l = [ (c & (1 << i)) for i in range(8)]
    print(chr(count + CHAR_OFFSET) + '---')
    print(' {} '.format(seg(order[0], '_', l)))
    print('{} {}'.format(seg(order[1], '|', l), seg(order[2], '|', l)))
    print(' {} '.format(seg(order[3], '-', l)))
    print('{} {}'.format(seg(order[4], '|', l), seg(order[5], '|', l)))
    print(' {} '.format(seg(order[6], '-', l)))
    print('----\n')
    count+=1

count = 0
for c in new_chars:
    print('{}, //{}'.format(format(c, '#010b'), chr(count + CHAR_OFFSET)))
    count += 1