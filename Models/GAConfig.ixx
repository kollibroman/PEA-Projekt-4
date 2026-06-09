export module GAConfig;

export enum class MutationMethod {
    Swap,
    Invert  // 2-opt
};

export enum class CrossoverMethod {
    OX,   // Order Crossover
    PMX   // Partially Mapped Crossover
};

export enum class SelectionMethod {
    Tournament,
    RouletteWheel
};

export struct GAConfig {
    int population_size = 100;
    double mutation_rate = 0.01;
    double crossover_rate = 0.8;
    double time_limit_seconds = 60.0;

    CrossoverMethod crossover_method = CrossoverMethod::OX;
    MutationMethod mutation_method = MutationMethod::Swap;
    SelectionMethod selection_method = SelectionMethod::Tournament;

    int tournament_size = 5;  // parametr selekcji turniejowej
};
