#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${PROJECT_ROOT}"

echo "Copie des jeux de données (samples) vers les répertoires d'entrée..."

mkdir -p data/input/web data/input/api

cp -v data/samples/sample_web_requests.csv data/input/web/
cp -v data/samples/sample_api_requests.csv data/input/api/

echo "Fichiers d'entrée prêts pour l'exécution !"
