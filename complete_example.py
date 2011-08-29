#!/usr/bin/env python
#-*- coding: utf-8 -*-
import sys

import nbio

class User(object):
    seq = 0
    objects = []
    def __init__(self, name, digital=''):
        self.name = name
        self.digital = digital

    @classmethod
    def find_by_name(cls, name):
        out = []
        for u in cls.objects:
            if u.name == name:
                out.append(u)
        return out

    @classmethod 
    def find_by_digital(cls, verify):
        for u in cls.objects:
            if verify(u.digital):
                return u

    def save(self):
        usr = User.find_by_name(self.name)
        if usr:
            raise ValueError('Este name já está sendo usado.')
        User.seq += 1
        self.id = User.seq
        User.objects.append(self)

    def __repr__(self):
        return '<User: %s>' % self.name

    def __str__(self):
        return self.name

def cadastrar():
    name = raw_input('Digite o nome do usuário: ')
    print 'Posicione o dedo no leitor biométrico'
    digital = nbio.enroll()
    new_user = User(name, digital)
    try:
        new_user.save()
    except ValueError as e:
        print e
    else:
        print 'Usuário %s cadastrado com sucesso.' % new_user

def verificar():
    print 'Posicione o dedo no leitor biométrico'
    user = User.find_by_digital(nbio.verify)
    if user:
        print 'Usuário encontrato:', user
    else:
        print 'Nenhum usuário encontrado.'
def sair():
    print 'Saindo...'
    nbio.close()
    sys.exit()

def main():
    menu = """
    Escolha uma das opções:
        1) Cadastrar
        2) Verificar
        3) Sair
    """
    menu_opts = {
        '1': cadastrar,
        '2': verificar,
        '3': sair,
    }
    nbio.init()
    nbio.open()
    while True:
        print menu
        menu_opt = raw_input('>>> ')
        menu_action = menu_opts.get(menu_opt)
        if menu_action is None:
            print 'Error: Opção inválida'
        else:
            menu_action()
if __name__ == '__main__':
    main()
