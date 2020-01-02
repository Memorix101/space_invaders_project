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

font = pygame.font.Font("rd/vermin_vibes_1989.ttf", 32)
player = Player((320, 450), pygame.image.load("rd/player.png"))
space_img = pygame.image.load("rd/space3.png")

lasers = list()
enemyLasers  = list()
enemies = list()

def spawn_enemies():
    for i in range(40):
        if i % 10 == 0:
            global itemCount
            itemCount = 0
            global rowCount
            rowCount = rowCount + 1
        itemCount = itemCount + 1
        rn = random.randrange(3, 20)
        enemies.append(Enemy((itemCount * 40, 60 * rowCount), pygame.image.load("rd/invader32x32x4.png"), itemCount, rn))

# create enemies
spawn_enemies()

# used to manage how fast the screen updates
clock = pygame.time.Clock()
 
# --- main loop
while not quit:
    # --- main event loop
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit = True
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE and player.alive == True:
                # shoot
                lasers.append(Bullet((player.pos_x, 400), pygame.image.load("rd/bullet.png"), False))
 
    # --- game logic
    player.update()

    for b in lasers: 
        b.update()                
        if b.pos_y <= 0:
            lasers.remove(b)
        for e in enemies: 
            if b.hitbox.colliderect(e.hitbox):
                enemies.remove(e)
                lasers.remove(b)
                score += 100
                break
    
    for e in enemies: 
        e.update()
        if e.shoot == True:
            enemyLasers.append(Bullet((e.pos_x, e.pos_y), pygame.image.load("rd/enemy-bullet.png"), True))
    
    for b in enemyLasers:
        b.update()
        if b.hitbox.colliderect(player.hitbox):
                player.alive = False
                enemyLasers.remove(b)
  
    # --- drawing
    # screen.fill((0, 0, 0))
    screen.blit(space_img, (0, 0))
    player.draw(screen)

    for b in lasers:
        b.draw(screen)    
        
    for b in enemyLasers:
        b.draw(screen)

    for e in enemies: 
        e.draw(screen)

    score_text = font.render('SCORE: %04d' % score, True, (255, 255, 255))
    screen.blit(score_text, (425,  25))    

    # pygame.display.flip()
    pygame.display.update()
 
    # limit to 60 fps
    clock.tick(60)
 
# close the window and quit
pygame.quit()