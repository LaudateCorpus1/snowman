/* The file is part of Snowman decompiler.             */
/* See doc/licenses.txt for the licensing information. */

/* * SmartDec decompiler - SmartDec is a native code to C/C++ decompiler
 * Copyright (C) 2015 Alexander Chernov, Katerina Troshina, Yegor Derevenets,
 * Alexander Fokin, Sergey Levin, Leonid Tsvetkov
 *
 * This file is part of SmartDec decompiler.
 *
 * SmartDec decompiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SmartDec decompiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SmartDec decompiler.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <nc/config.h>

#include <vector>

namespace nc {
namespace core {

namespace arch {
    class Architecture;
}

namespace ir {

class Function;
class Jump;
class Statement;
class Term;

namespace calling {
    class Hooks;
    class Signatures;
}

namespace cflow {
    class Graph;
}

namespace dflow {
    class Dataflow;
}

namespace liveness {

class Liveness;

/**
 * This class computes the set of terms which compute values actually used
 * in the high-level code to be generated.
 *
 * Note that this differs from the classic liveness analysis. Although a term
 * may be live in the classic definition sense, it may be not used for
 * generating actual code and therefore be dead in our sense. For example,
 * stack pointer updates do not appear in the generated code, as long as
 * offsets from the frame base can be inferred.
 */
class LivenessAnalyzer {
    Liveness &liveness_; ///< Liveness information.
    const Function *function_; ///< Function to be analyzed.
    const dflow::Dataflow &dataflow_; ///< Dataflow information.
    const arch::Architecture *architecture_; ///< Architecture.
    const cflow::Graph &regionGraph_; ///< Reduced control-flow graph.
    calling::Hooks &hooks_; ///< Calls data.
    const calling::Signatures &signatures_; ///< Signatures of functions.
    std::vector<const Jump *> uselessJumps_; ///< Useless jumps.

public:
    /**
     * Constructor.
     *
     * \param[out] liveness     Liveness information.
     * \param[in]  function     Valid pointer to a function to be analyzed.
     * \param[in]  dataflow     Dataflow information.
     * \param[in]  architecture Valid pointer to the architecture.
     * \param[in]  regionGraph  Reduced control-flow graph.
     * \param[in]  hooks        Calls data.
     * \param[in]  signatures   Signatures of functions.
     */
    LivenessAnalyzer(Liveness &liveness, const Function *function,
        const dflow::Dataflow &dataflow, const arch::Architecture *architecture, 
        const cflow::Graph &regionGraph, calling::Hooks &hooks,
        const calling::Signatures &signatures
    ):
        liveness_(liveness), function_(function), dataflow_(dataflow),
        architecture_(architecture), regionGraph_(regionGraph),
        hooks_(hooks), signatures_(signatures)
    {}

    /**
     * Virtual destructor.
     */
    virtual ~LivenessAnalyzer() {}

    /**
     * \return Liveness information.
     */
    Liveness &liveness() { return liveness_; }

    /**
     * \return Liveness information.
     */
    const Liveness &liveness() const { return liveness_; }

    /**
     * \return Valid pointer to the function being analyzed.
     */
    const Function *function() const { return function_; }

    /**
     * \return Valid pointer to the dataflow information.
     */
    const dflow::Dataflow &dataflow() const { return dataflow_; }

    /**
     * \return Valid pointer to the architecture.
     */
    const arch::Architecture *architecture() const { return architecture_; }

    /**
     * \return Pointer to the reduced control-flow graph.
     */
    const cflow::Graph &regionGraph() const { return regionGraph_; }

    /**
     * \return Calls data.
     */
    calling::Hooks &hooks() const { return hooks_; }

    /**
     * \return Signatures of functions.
     */
    const calling::Signatures &signatures() const { return signatures_; }

    /**
     * Computes the set of used terms.
     */
    void analyze();

protected:
    /**
     * Computes liveness of statement's terms based on the statement's kind.
     *
     * \param[in] statement Statement.
     */
    virtual void computeLiveness(const Statement *statement);

    /**
     * Computes liveness of a term based on the term's kind.
     *
     * \param[in] term Term to consider.
     */
    virtual void computeLiveness(const Term *term);

    /**
     * Marks as used all the terms, used by given term in order to generate code.
     *
     * \param[in] term Used term.
     */
    virtual void propagateLiveness(const Term *term);

    /**
     * If given term is not used, marks it as used and propagates liveness further.
     *
     * \param[in] term Term.
     */
    void makeLive(const Term *term);
};

} // namespace liveness
} // namespace ir
} // namespace core
} // namespace nc

/* vim:set et sts=4 sw=4: */