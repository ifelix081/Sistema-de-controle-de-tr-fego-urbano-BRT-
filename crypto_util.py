#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Utilitário para criptografar/descriptografar arquivos de configuração
com senhas.

Uso:
    python3 crypto_util.py encrypt src/config/admin.txt
    python3 crypto_util.py decrypt src/config/admin.txt.enc
    python3 crypto_util.py show src/config/admin.txt.enc
"""

import os
import sys
import json
import hashlib
import hmac
from cryptography.fernet import Fernet, InvalidToken
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2
from cryptography.hazmat.backends import default_backend
import base64
import getpass
from pathlib import Path

class ConfigCrypto:
    """Gerenciador de criptografia para arquivos de configuração"""
    
    HEADER = "BRT_ENC_V1"  # Identificador de versão
    ITERATIONS = 100000    # PBKDF2 iterations
    
    def __init__(self, password=None):
        """
        Inicializar com senha
        Se password=None, pede interativamente
        """
        if password is None:
            self.password = getpass.getpass("🔐 Digite a senha de criptografia: ")
        else:
            self.password = password
            
        if not self.password:
            raise ValueError("Senha não pode estar vazia")
    
    def _derive_key(self, salt):
        """Derivar chave Fernet a partir da senha"""
        kdf = PBKDF2(
            algorithm=hashes.SHA256(),
            length=32,
            salt=salt,
            iterations=self.ITERATIONS,
            backend=default_backend()
        )
        key = base64.urlsafe_b64encode(kdf.derive(self.password.encode()))
        return key
    
    def encrypt_file(self, input_file, output_file=None):
        """Criptografar arquivo de configuração"""
        
        if not os.path.exists(input_file):
            print(f"❌ Erro: arquivo não encontrado: {input_file}")
            return False
        
        if output_file is None:
            output_file = input_file + ".enc"
        
        try:
            # Ler arquivo original
            with open(input_file, 'rb') as f:
                plaintext = f.read()
            
            # Gerar salt aleatório
            salt = os.urandom(16)
            
            # Derivar chave
            key = self._derive_key(salt)
            
            # Criptografar
            cipher = Fernet(key)
            ciphertext = cipher.encrypt(plaintext)
            
            # Formato: HEADER|SALT_B64|CIPHERTEXT_B64
            salt_b64 = base64.b64encode(salt).decode('utf-8')
            ciphertext_b64 = base64.b64encode(ciphertext).decode('utf-8')
            
            output_data = f"{self.HEADER}|{salt_b64}|{ciphertext_b64}"
            
            # Salvar arquivo criptografado
            with open(output_file, 'w') as f:
                f.write(output_data)
            
            # Definir permissões restritivas (600)
            os.chmod(output_file, 0o600)
            
            print(f"✅ Arquivo criptografado com sucesso!")
            print(f"   Original:       {input_file}")
            print(f"   Criptografado:  {output_file}")
            print(f"   Permissões:     600 (rw------)")
            print(f"   Tamanho:        {len(plaintext)} → {len(output_data)} bytes")
            
            return True
            
        except Exception as e:
            print(f"❌ Erro ao criptografar: {e}")
            return False
    
    def decrypt_file(self, input_file, output_file=None):
        """Descriptografar arquivo de configuração"""
        
        if not os.path.exists(input_file):
            print(f"❌ Erro: arquivo não encontrado: {input_file}")
            return False
        
        if output_file is None:
            output_file = input_file.replace('.enc', '')
        
        try:
            # Ler arquivo criptografado
            with open(input_file, 'r') as f:
                content = f.read()
            
            # Parse: HEADER|SALT_B64|CIPHERTEXT_B64
            parts = content.split('|')
            if len(parts) != 3 or parts[0] != self.HEADER:
                print(f"❌ Erro: formato de arquivo inválido")
                return False
            
            salt_b64 = parts[1]
            ciphertext_b64 = parts[2]
            
            # Decodificar
            salt = base64.b64decode(salt_b64)
            ciphertext = base64.b64decode(ciphertext_b64)
            
            # Derivar chave
            key = self._derive_key(salt)
            
            # Descriptografar
            try:
                cipher = Fernet(key)
                plaintext = cipher.decrypt(ciphertext)
            except InvalidToken:
                print(f"❌ Erro: senha incorreta ou arquivo corrompido")
                return False
            
            # Salvar arquivo descriptografado
            with open(output_file, 'wb') as f:
                f.write(plaintext)
            
            # Definir permissões restritivas (600)
            os.chmod(output_file, 0o600)
            
            print(f"✅ Arquivo descriptografado com sucesso!")
            print(f"   Criptografado:  {input_file}")
            print(f"   Original:       {output_file}")
            print(f"   Permissões:     600 (rw------)")
            
            return True
            
        except Exception as e:
            print(f"❌ Erro ao descriptografar: {e}")
            return False
    
    def show_info(self, input_file):
        """Mostrar informações sobre arquivo criptografado"""
        
        if not os.path.exists(input_file):
            print(f"❌ Erro: arquivo não encontrado: {input_file}")
            return False
        
        try:
            with open(input_file, 'r') as f:
                content = f.read()
            
            parts = content.split('|')
            if len(parts) != 3:
                print(f"❌ Erro: formato de arquivo inválido")
                return False
            
            # Estatísticas
            stat = os.stat(input_file)
            
            print(f"\n📄 Informações do Arquivo Criptografado")
            print(f"{'='*50}")
            print(f"Arquivo:        {input_file}")
            print(f"Tamanho:        {stat.st_size} bytes")
            print(f"Versão:         {parts[0]}")
            print(f"Permissões:     {oct(stat.st_mode)[-3:]}")
            print(f"Salt (hex):     {base64.b64decode(parts[1]).hex()[:32]}...")
            print(f"Ciphertext:     {len(base64.b64decode(parts[2]))} bytes criptografados")
            print(f"{'='*50}\n")
            
            return True
            
        except Exception as e:
            print(f"❌ Erro ao ler arquivo: {e}")
            return False


def main():
    """Programa principal"""
    
    if len(sys.argv) < 3:
        print("🔐 Utilitário de Criptografia - Sistema BRT")
        print()
        print("Uso:")
        print("  python3 crypto_util.py encrypt <arquivo>")
        print("  python3 crypto_util.py decrypt <arquivo.enc>")
        print("  python3 crypto_util.py show <arquivo.enc>")
        print()
        print("Exemplos:")
        print("  python3 crypto_util.py encrypt src/config/admin.txt")
        print("    → Cria: src/config/admin.txt.enc (criptografado)")
        print()
        print("  python3 crypto_util.py decrypt src/config/admin.txt.enc")
        print("    → Cria: src/config/admin.txt (descriptografado)")
        print()
        print("  python3 crypto_util.py show src/config/admin.txt.enc")
        print("    → Mostra informações do arquivo")
        print()
        return 1
    
    comando = sys.argv[1].lower()
    arquivo = sys.argv[2]
    
    try:
        crypto = ConfigCrypto()
        
        if comando == "encrypt":
            sucesso = crypto.encrypt_file(arquivo)
            return 0 if sucesso else 1
            
        elif comando == "decrypt":
            sucesso = crypto.decrypt_file(arquivo)
            return 0 if sucesso else 1
            
        elif comando == "show":
            sucesso = crypto.show_info(arquivo)
            return 0 if sucesso else 1
            
        else:
            print(f"❌ Comando desconhecido: {comando}")
            return 1
            
    except KeyboardInterrupt:
        print("\n❌ Operação cancelada pelo usuário")
        return 1
    except Exception as e:
        print(f"❌ Erro: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
