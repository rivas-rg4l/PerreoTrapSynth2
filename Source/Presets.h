#pragma once

#include <array>
#include <utility>
#include <vector>

//==============================================================================
// Organizacion de presets:  2 divisiones x 6 categorias x 5 presets = 60
//
//   indice = genero * 30 + categoria * 5 + slot
//==============================================================================
namespace PresetLayout
{
    constexpr int numGenres     = 2;
    constexpr int numCategories = 6;
    constexpr int perCategory   = 5;
    constexpr int perGenre      = numCategories * perCategory;
    constexpr int total         = numGenres * perGenre;

    inline const char* genreName (int g)
    {
        static const char* const names[] = { "TRAP", "REGGAETON" };
        return names[g < 0 ? 0 : (g >= numGenres ? numGenres - 1 : g)];
    }

    inline const char* categoryName (int c)
    {
        static const char* const names[] = { "PLUCKS", "LEADS", "PADS", "KEYS", "SYNTHS", "CHOIRS" };
        return names[c < 0 ? 0 : (c >= numCategories ? numCategories - 1 : c)];
    }

    inline int genreOf    (int index) { return index / perGenre; }
    inline int categoryOf (int index) { return (index % perGenre) / perCategory; }
    inline int slotOf     (int index) { return index % perCategory; }
    inline int indexOf    (int g, int c, int s) { return g * perGenre + c * perCategory + s; }
}

//==============================================================================
// Un preset es solo una lista de (idParametro, valor). Todo lo que no se
// nombre vuelve a su valor por defecto, asi los presets nunca "heredan" cosas
// del sonido anterior.
//==============================================================================
struct PerreoPreset
{
    using Values = std::vector<std::pair<const char*, float>>;

    const char* name;
    Values      values;
};

const std::vector<PerreoPreset>& getPresetTable();
