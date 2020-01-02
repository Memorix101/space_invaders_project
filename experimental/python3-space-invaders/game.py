# Python 3.7.6
# pip install pygame

import pygame
import os
import random

from player import Player
from bullet import Bullet
from enemy import Enemy

# define some colours
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
 
pygame.init()
os.environ['SDL_VIDEO_CENTERED'] = '1'
screen = pygame.display.set_mode((640, 480)) 
pygame.display.set_caption("Space Invaders pygame")

# setup stuff
quit = False
score = 0
rowCount = 0
itemCount = 0
gameStart = False
gameTimer = 0.0
gameOver = False

font = pygame.font.Font("rd/vermin_vibes_1989.ttf", 32)
player = Player((640.0 / 2.0 - 28 / 2.0, 450), pygame.image.load("rd/player.png"))
space_img = pygame.image.load("rd/space3.png")
win_img = pygame.image.load("rd/win_ui.png")
gameover_img = pygame.image.load("rd/gameover_ui.png")
fmg_img = pygame.image.load("rd/fmg_splash.png")

lasers = list()
enemyLasers  = list()
enemies = list()
explosions = list()

def spawnEnemies():
    for i in range(40):
        if i % 10 == 0:
            global itemCount
            itemCount = 0
            global rowCount
            rowCount = rowCount + 1
        itemCount = itemCount + 1
        rn = random.randrange(3, 20)
        enemies.append(Enemy((itemCount * 40, 60 * rowCount), pygame.image.load("rd/invader32x32x4.png"), itemCount, rn))

def restart ():
    player.alive = True
    player.pos = (640.0 / 2.0 - player.hitbox.w / 2.0, player.pos_y)
    lasers.clear()
    enemies.clear()
    enemyLasers.clear()
    explosions.clear()
    global itemCount
    itemCount = 0
    global rowCount
    rowCount = 0
    global score
    score = 0
    global gameOver
    gameOver = False
    spawnEnemies()

# create enemies
spawnEnemies()

# used to manage how fast the screen updates
clock = pygame.time.Clock()
 
# --- main loop
while not quit:
    # --- main event loop
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit = True
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE and gameOver == False:
                lasers.append(Bullet((player.pos_x, 400), pygame.image.load("rd/bullet.png"), False))
            if event.key == pygame.K_RETURN and gameOver == True:
                restart()
 
    # --- game logic
    player.update()

    for l in lasers: 
        l.update()                
        if l.pos_y <= 0:
            lasers.remove(l)
        for e in enemies: 
            if l.hitbox.colliderect(e.hitbox):
                enemies.remove(e)
                lasers.remove(l)
                score += 100
                break
    
    for e in enemies: 
        e.update()
        if e.shoot == True:
            enemyLasers.append(Bullet((e.pos_x, e.pos_y), pygame.image.load("rd/enemy-bullet.png"), True))
    
    for l in enemyLasers:
        l.update()
        if l.hitbox.colliderect(player.hitbox):
                player.alive = False
                enemyLasers.remove(l)
  
    # --- drawing
    # screen.fill((0, 0, 0))
    screen.blit(space_img, (0, 0))
    player.draw(screen)

    for l in lasers:
        l.draw(screen)    
        
    for l in enemyLasers:
        l.draw(screen)

    for e in enemies: 
        e.draw(screen)

    score_text = font.render('SCORE: %04d' % score, True, (255, 255, 255))
    screen.blit(score_text, (425,  25))

    if player.alive == False:
        screen.blit(gameover_img, (0, 0))
        gameOver = True
    
    if len(enemies) == 0:
        screen.blit(win_img, (0, 0))
        gameOver = True

    # pygame.display.flip()
    pygame.display.update()
 
    # limit to 60 fps
    clock.tick(60)
 
# close the window and quit
pygame.quit()