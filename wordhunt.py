import sys
import pygame
import pygame.freetype
import random
import math
import numpy as np
from sortedcontainers import SortedSet
from collections import OrderedDict
#from sets import Set
from collections import defaultdict
from itertools import product
from bisect import bisect_left

pygame.init()

window_width, window_height = 800, 800
window_color = (22, 22, 22)
letter_color = (250, 122, 250)
screen = pygame.display.set_mode((window_width, window_height))
pygame.display.set_caption("Word Hunt")
font_file = 'arial.ttf'
letter_font = pygame.freetype.Font(font_file, 80)

def read_letter_frequencies(filename):
    frequencies = [0 for i in range(26)]
    with open(filename, 'r') as file:
        for line in file:
            letter, frequency = line.split()
            frequencies[ord(letter) - ord('A')] = float(frequency)
    return frequencies

filename = 'letterdata.txt'
letter_weights = read_letter_frequencies(filename)

def generate_random_letter():
    letters = list('abcdefghijklmnopqrstuvwxyz'.upper()) 
    return random.choices(letters, weights=letter_weights)[0]

board_cols, board_rows = 4, 4
tile_width, tile_height = window_width / board_cols, window_height / board_rows
board = [['A' for _ in range(board_cols)] for i in range(board_rows)]
def generate_board():
    for i in range(board_rows):
        for j in range(board_cols):
            board[i][j] = generate_random_letter()
generate_board()

def find_tile_center(row, col):
    x, y = col * tile_width + tile_width/2, row * tile_height + tile_height/2, 
    return (x, y)

def display_letter(row, col):
    letter = board[row][col]
    x, y = find_tile_center(row, col)
    letter_surface, letter_rect = letter_font.render(letter, fgcolor=letter_color)
    letter_rect.center = (x, y)
    screen.blit(letter_surface, letter_rect)

highlight_color = (200, 200, 200, 255)
def highlight_letter(row, col):
    tile_rect = pygame.Rect(row*tile_width, col*tile_height, tile_width, tile_height)
    pygame.draw.rect(screen, highlight_color, tile_rect, 3)

def read_words(filename):
    with open(filename, 'r') as file:
        words = file.readlines()
    words = {word.strip() for word in words}
    return words

filename = 'CSW21.txt' # used to be https://github.com/wordnik/wordlist
words = read_words(filename)
# words = sorted(words)

def find_neighbors(row, col):
    neighbors = []
    #left, right, top, bottom = (row==0), (row==board_rows-1), (col==0), (col==board_cols-1)
    for i in range(row - 1, row + 2):
        for j in range(col - 1, col + 2):
            if not (i == row and j == col) and 0 <= i < board_rows and 0 <= j < board_cols:
                #if (i == row - 1 and not top) or (i == row + 1 and not bottom) or (j == col - 1 and not left) or (j == col + 1 and not right):
                neighbors.append((i, j))
    return neighbors

def coord_to_index(row, col):
    return row * board_cols + col
def index_to_coord(i):
    return (int(i) // board_cols, int(i) % board_cols)
def coord_to_mask(row, col):
    return (1 << coord_to_index(row, col))
def mask_to_coord(i):
    return index_to_coord(math.log2(i))

"""# def generate_combos(curr_list, curr_length): # 16 lists of words, one per tile. word = [0000, 0100, 1100, ...] giving the letter positions
#     # curr_list = X[rows][cols] = [words], eg X[1][2] = ["A", "AB"]]
#     if curr_length == board_rows*board_cols:
#         return curr_list
#     for row in range(board_rows):
#         for col in range(board_cols):
#             neighbors = find_neighbors(row, col)
#             for n in neighbors:
#                 neighbor_row, neighbor_col = n
#                 nw = len(curr_list[neighbor_row][neighbor_col])
#                 for c in range(nw):
#                     combo = curr_list[neighbor_row][neighbor_col][c]
#                     print(combo)
#                     curr_list[neighbor_row][neighbor_col].append(combo + board[row][col]) # not finished
#     return generate_combos(curr_list, curr_length+1)
#print(generate_combos([[[board[j][i]] for i in range(board_cols)] for j in range(board_rows)], 0))"""
def generate_combos(target_length): # 16 lists of words, one per tile. word = [0000, 0100, 1100, ...] giving the letter positions
    # curr_list = X[rows][cols] = [words], eg X[1][2] = ["A", "AB"]]
    curr_list = [[[(board[i][j], coord_to_mask(i, j))] for j in range(board_cols)] for i in range(board_rows)]
    #curr_list = [[[([coord_to_mask(i, j)], coord_to_mask(i, j))] for j in range(board_cols)] for i in range(board_rows)]
    if target_length == 1:
        return curr_list
    shorter = generate_combos(target_length-1)
    for row in range(board_rows):
        for col in range(board_cols): # O(RC)
            neighbors = find_neighbors(row, col)
            for n in neighbors: # O(K)
                neighbor_row, neighbor_col = n
                for short in shorter[row][col]: # O((RC)!)
                    neighbor_mask = coord_to_mask(neighbor_row, neighbor_col)
                    if neighbor_mask & short[1]:
                        continue
                    #curr_list[neighbor_row][neighbor_col].append((short[0] + [neighbor_mask], short[1] | neighbor_mask)) # not finished
                    curr_list[neighbor_row][neighbor_col].append((board[neighbor_row][neighbor_col] + short[0], short[1] | neighbor_mask)) 
    return curr_list
    # O(RCK)
#print(generate_combos(6))
#print(generate_combos(2))
generated = defaultdict(lambda: defaultdict(lambda: defaultdict(set)))
def generate_combos_dp(target_length):
    for row in range(board_rows):
        for col in range(board_cols):
            generated[0][row][col].add((board[row][col], coord_to_mask(row, col)))

    for i in range(1, target_length):
        for row in range(board_rows):
            for col in range(board_cols):
                neighbors = find_neighbors(row, col)
                for neighbor_row, neighbor_col in neighbors:
                    neighbor_mask = coord_to_mask(neighbor_row, neighbor_col)
                    for short_item, mask in generated[i - 1][row][col]:
                        if neighbor_mask & mask:
                            continue
                        closest_next_word = next(w for w in words if w >= short_item)
                        if len(closest_next_word) < i or closest_next_word[i - 1] != short_item[-1]:
                            continue
                        generated[i][neighbor_row][neighbor_col].add((short_item + board[neighbor_row][neighbor_col], mask | neighbor_mask))

def mask_to_word(mask):
    decoded = ""
    for i in mask:
        r, c = mask_to_coord(i)
        decoded += board[r][c]
    return decoded

def is_valid(word):
    # word = [512, 4096, 128]
    if word.lower() in words: #mask_to_word(word).lower() in words:
        print(word)
        return True
    return False

valid_words = []
def solve_board(k):
    # valid_words = []
    # combos = generate_combos_dp(2)
    # for row in range(board_rows):
    #     for col in range(board_cols):
    #         combos[row][col] = list(OrderedDict.fromkeys(combos[row][col]))
    #         print(combos)
    #         for combo in combos[row][col]:
    #             if is_valid(combo[0]):
    #                 valid_words.append(combo[0])
    generate_combos_dp(k)
    for i in range(k): 
        for row in range(board_rows): 
            for col in range(board_cols): 
                for combo in generated[i][row][col]:
                    word = combo[0]
                    if word in words:
                        valid_words.append(combo[0])
solve_board(4)
for w in valid_words:
    print(w)

def update_board():
    screen.fill(window_color)
    for i in range(board_cols):
        for j in range(board_rows):
            display_letter(j, i)

    for n in find_neighbors(0, 0):
        highlight_letter(n[0], n[1])
    pygame.display.flip()

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    update_board()