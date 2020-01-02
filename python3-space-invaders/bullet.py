import pygame

class Bullet:
    def __init__(self, pos, tex2d, isEnemy):
        self.pos = pos
        self.pos_x = pos[0]
        self.pos_y = pos[1]
        self.tex2d = tex2d
        self.isEnemy = isEnemy

        if self.isEnemy == True:
            self.hitbox = pygame.Rect(0, 0, 6, 36)
        else:
            self.hitbox = pygame.Rect(0, 0, 9, 9)

    def update(self):
        self.hitbox = pygame.Rect(self.pos_x, self.pos_y, self.hitbox.w, self.hitbox.h)
        
        if self.isEnemy == True:
            self.pos_y += 10
        else:
            self.pos_y -= 10
    
    def draw(self, screen):
            screen.blit(self.tex2d, (self.pos_x, self.pos_y))