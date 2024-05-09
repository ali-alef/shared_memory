import os

from dotenv import dotenv_values

ENVIRONMENT = os.environ.get('ENV', 'local').lower()
env_vars = dotenv_values(f'./config/{ENVIRONMENT}.env')

DLL_PATH = env_vars.get('DLL_PATH')
print(DLL_PATH)
