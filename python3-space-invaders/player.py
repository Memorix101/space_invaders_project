import pygame

class Player:
    
    alive = True

    def __init__(self, pos, tex2d):
        self.pos = pos
        self.pos_x = pos[0]
        self.pos_y = pos[1]
        self.tex2d = tex2d
        self.hitbox = pygame.Rect(0, 0, 28, 21)

    def update(self):
        if self.alive == True:
            # input stuff
            key = pygame.key.get_pressed()  #checking pressed keys        
            if key[pygame.K_LEFT] or key[pygame.K_a]:
                self.pos_x -= 5
            if key[pygame.K_RIGHT] or key[pygame.K_d]:
                self.pos_x += 5

            # other stuff
            self.hitbox = pygame.Rect(self.pos_x, self.pos_y, self.hitbox.w, self.hitbox.h)
            self.pos_x = max(min(self.pos_x, 610), 0)

    def draw(self, screen):
        if self.alive == True:
            screen.blit(self.tex2d, (self.pos_x, self.pos_y))