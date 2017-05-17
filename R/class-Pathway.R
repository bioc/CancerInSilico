#' @include class-CellModel.R
NULL

library(methods)

################ Class Definition ################

setClass('Pathway', slots = c(
    genes = 'character',
    expressionScale = 'function',
    minExpression = 'numeric',    
    maxExpression = 'numeric'    
))

setMethod('initialize', 'Pathway',
    function(.Object, ...)
    {
        if (is.null(list(...)$genes))
            stop('missing genes')
        if (is.null(list(...)$expressionScale))
            stop('missing expressionScale')
        .Object <- callNextMethod(.Object, ...) 
        return(.Object)
    }
)

setValidity('Pathway',
    function(object)
    {
        if (!length(object@genes))
            'no genes provided'
        else if (!length(object@maxExpression))
            'missing \'maxExpression\''
        else if (!length(object@minExpression))
            'missing \'minExpression\''
        else if (object@maxExpression <= object@minExpression)
            'invalid expression range'
        else if (object@maxExpression <= 0)
            '\'maxExpression\' must be positive'
        else if (object@minExpression < 0)
            '\'minExpression\' must be non-negative'
        else TRUE
    }
)

##################### Generics ###################

setGeneric('simulateExpression',
    function(pathway, model, sampFreq, singleCell = FALSE, sampSize = 0)
        {standardGeneric('simulateExpression')})

##################### Methods ####################

setMethod('simulateExpression',
    signature(pathway = 'Pathway', model = 'CellModel'),
    function(pathway, model, sampFreq, singleCell, sampSize)
    {
        # check parameters when doing single cell
        if (!singleCell) {sampSize <- 1}
        else if (sampSize < 1) stop('invalid sample size for single cell')
    
        # find closest, valid, sampling frequency
        sampFreq <- model@recordIncrement *
            ceiling(sampFreq / model@recordIncrement)

        # get number of time points and create the return matrix
        times <- seq(0, model@runTime, sampFreq)
        gsMatrix <- matrix(0, length(pathway@genes), length(times)*sampSize)
        colnames(gsMatrix) <- rep(times, each = sampSize)
        rownames(gsMatrix) <- pathway@genes

        # loop through each time
        for (t in 1:length(times))
        {
            # determine which cells to calculate expression for
            cells <- 1:getNumberOfCells(model, times[t])
            if (singleCell) cells <- sort(sample(cells, sampSize))

            # calculate gene expression
            scale <- sapply(cells, function(c)
                pathway@expressionScale(model, c, times[t]))
            exp <- (pathway@max - pathway@min) %*% t(scale) + pathway@min
                        
            # add expression to matrix
            cols <- (sampSize * (t-1) + 1):(sampSize * t)            
            if (singleCell) gsMatrix[,cols] <- exp
            else gsMatrix[,cols] <- mean(exp)
        }
        return(gsMatrix)
    }
)