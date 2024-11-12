import subprocess
import sys

def check_and_install_requirements():
    try:
        import pkg_resources
    except ImportError:
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'setuptools'])
        import pkg_resources

    requirements_file = 'requirements.txt'
    with open(requirements_file, 'r') as file:
        requirements = [line.strip() for line in file if line.strip()]

    for requirement in requirements:
        try:
            pkg_resources.require(requirement)
        except pkg_resources.DistributionNotFound:
            subprocess.check_call([sys.executable, '-m', 'pip', 'install', requirement])
        except pkg_resources.VersionConflict:
            subprocess.check_call([sys.executable, '-m', 'pip', 'install', requirement])

if __name__ == "__main__":
    check_and_install_requirements()
    # Coloque o resto do seu script aqui
