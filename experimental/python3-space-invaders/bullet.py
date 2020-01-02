import pygame

class Bullet:
    def __init__(self, pos, tex2d):
        self.pos = pos
        self.pos_x = pos[0]
        self.pos_y = pos[1]
        self.tex2d = tex2d
        self.hitbox = pygame.Rect(0, 0, 6, 36)

    def update(self):
        self.hitbox = pygame.Rect(self.pos_x, self.pos_y, 6, 36)
        self.pos_y -= 10
    
    def draw(self, screen):
            screen.blit(self.tex2d, (self.pos_x, self.pos_y))