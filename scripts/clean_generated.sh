#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${PROJECT_ROOT}"

echo "Nettoyage des fichiers générés et artefacts d'exécution..."

# Suppression du contenu des dossiers générés/transitoires
rm -rf data/output/*
rm -rf data/quarantine/*
rm -rf data/rejected/*
rm -rf data/archive/*
rm -rf data/staging/*
rm -rf data/input/web/* data/input/api/* 2>/dev/null || true
rm -rf logs/*

# Recréation de l'arborescence requise
mkdir -p data/output \
         data/quarantine \
         data/rejected/web data/rejected/api \
         data/archive/web data/archive/api \
         data/staging/web data/staging/api \
         data/input/web data/input/api \
         logs

echo "Nettoyage terminé avec succès !"
