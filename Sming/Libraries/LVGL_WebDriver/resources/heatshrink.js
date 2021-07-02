// basic.js
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var HSState;
(function (HSState) {
    HSState[HSState["TAG_BIT"] = 0] = "TAG_BIT";
    HSState[HSState["YIELD_LITERAL"] = 1] = "YIELD_LITERAL";
    HSState[HSState["BACKREF_INDEX_MSB"] = 2] = "BACKREF_INDEX_MSB";
    HSState[HSState["BACKREF_INDEX_LSB"] = 3] = "BACKREF_INDEX_LSB";
    HSState[HSState["BACKREF_COUNT_MSB"] = 4] = "BACKREF_COUNT_MSB";
    HSState[HSState["BACKREF_COUNT_LSB"] = 5] = "BACKREF_COUNT_LSB";
    HSState[HSState["YIELD_BACKREF"] = 6] = "YIELD_BACKREF";
})(HSState || (HSState = {}));
/**
 * All errors thrown by the heatshrink-ts package will inherit from
 * this class.  Different subclasses are thrown for different kinds of
 * errors.  All errors have a string message that indicates what exactly
 * went wrong.
 *
 * @category Errors
 */
var HSError = /** @class */ (function (_super) {
    __extends(HSError, _super);
    function HSError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return HSError;
}(Error));

/**
 * The heatshrink engine has been misconfigured.
 *
 * @category Errors
 */
var HSConfigError = /** @class */ (function (_super) {
    __extends(HSConfigError, _super);
    function HSConfigError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return HSConfigError;
}(HSError));

var HSInternalError = /** @class */ (function (_super) {
    __extends(HSInternalError, _super);
    function HSInternalError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return HSInternalError;
}(HSError));

var HSCorruptDataError = /** @class */ (function (_super) {
    __extends(HSCorruptDataError, _super);
    function HSCorruptDataError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return HSCorruptDataError;
}(HSError));

var HS_MIN_WINDOW_BITS = 4;
var HS_MAX_WINDOW_BITS = 15;
var HS_MIN_LOOKAHEAD_BITS = 3;
var HS_LITERAL_MARKER = 0x01;
var HS_BACKREF_MARKER = 0x00;
var HS_NOBITS = -1;


// decoder.js
/**
 * A typescript implementation of the heatshrink compression library.
 *
 * Heatshrink is an open-source LZSS based compression library suitable
 * for use in embedded systems since it has a very small and bounded
 * memory footprint.  This is an adaptation of the heatshink code to
 * typescript with a slightly more user-fiendly API.
 */
var HeatshrinkDecoder = /** @class */ (function () {
    function HeatshrinkDecoder(windowBits, lookaheadBits, inputBufferSize) {
        this.outputSize = 0;
        this.headIndex = 0;
        this.outputIndex = 0;
        this.outputCount = 0;
        this.state = HSState.TAG_BIT;
        this.inputState = {
            size: 0,
            index: 0,
            currentByte: 0,
            bitIndex: 0
        };
        if (lookaheadBits >= windowBits) {
            throw new HSConfigError("Invalid lookahead size (" + lookaheadBits + ") that is not smaller than the specified window " + windowBits);
        }
        else if (lookaheadBits <= 0 || windowBits <= 0 || inputBufferSize <= 0) {
            throw new HSConfigError("Invalid lookahead (" + lookaheadBits + "), window (" + windowBits + ") or input (" + inputBufferSize + ") size that must be greater than 0");
        }
        else if (windowBits < HS_MIN_WINDOW_BITS || windowBits > HS_MAX_WINDOW_BITS) {
            throw new HSConfigError("Invalid window bit size that is not in [" + HS_MIN_WINDOW_BITS + ", " + HS_MAX_WINDOW_BITS + "]");
        }
        this.windowBits = windowBits;
        this.lookaheadBits = lookaheadBits;
        this.inputBuffer = new Uint8Array(inputBufferSize);
        this.outputBuffer = new Uint8Array(0);
        this.windowBuffer = new Uint8Array(Math.pow(2, this.windowBits));
        this.reset();
    }
    HeatshrinkDecoder.prototype.reset = function () {
        this.inputState = {
            size: 0,
            index: 0,
            currentByte: 0,
            bitIndex: 0
        };
        this.state = HSState.TAG_BIT;
        this.headIndex = 0;
        this.outputIndex = 0;
        this.outputCount = 0;
        this.outputSize = 0;
        this.inputBuffer.fill(0);
        this.windowBuffer.fill(0);
        this.outputBuffer.fill(0);
    };
    /**
     * Feed data into the heatshrink decoder state machine.
     *
     * This function will take the chunk of input data and turn it into as
     * much expanded output as it can.  Decoding a stream of data should be
     * done by calling this function repeatedly with chunks of data from the
     * stream.
     *
     * You can call isFinished() to check and see if all of the data that you
     * have fed in from previous calls to process() has been successfully
     * decoded.
     *
     * @param rawInput A chunk of data that has encoded using the heatshrink
     *      library.  You can push data a little bit at a time and stop at
     *      any byte boundary.
     */
    HeatshrinkDecoder.prototype.process = function (rawInput) {
        var input = this.assureUint8Array(rawInput);
        while (input.byteLength > 0) {
            var remaining = this.sink(input);
            this.poll();
            input = input.slice(input.byteLength - remaining);
        }
    };
    HeatshrinkDecoder.prototype.sink = function (input) {
        var remaining = this.inputBuffer.byteLength - this.inputState.size;
        var copySize = input.byteLength;
        if (copySize > remaining) {
            copySize = remaining;
        }
        this.inputBuffer.set(input.slice(0, copySize), this.inputState.size);
        this.inputState.size += copySize;
        return input.byteLength - copySize;
    };
    HeatshrinkDecoder.prototype.poll = function () {
        while (true) {
            var inState = this.state;
            switch (inState) {
                case HSState.TAG_BIT:
                    this.state = this.processTag();
                    break;
                case HSState.YIELD_LITERAL:
                    this.state = this.yieldLiteral();
                    break;
                case HSState.BACKREF_COUNT_MSB:
                    this.state = this.processBackrefCountMSB();
                    break;
                case HSState.BACKREF_COUNT_LSB:
                    this.state = this.processBackrefCountLSB();
                    break;
                case HSState.BACKREF_INDEX_MSB:
                    this.state = this.processBackrefIndexMSB();
                    break;
                case HSState.BACKREF_INDEX_LSB:
                    this.state = this.processBackrefIndexLSB();
                    break;
                case HSState.YIELD_BACKREF:
                    this.state = this.yieldBackref();
                    break;
            }
            /*
             * If our state didn't change, we can't process any more input data so return.
             */
            if (this.state === inState) {
                return;
            }
        }
    };
    HeatshrinkDecoder.prototype.isFinished = function () {
        return this.inputState.size === 0;
    };
    /**
     * Get all output data and truncate the output buffer.
     *
     * Calling this function repeatedly will have the effect of
     * pulling the output in chunks from the HeatshrinkDecoder.
     * It will return all data currently available and remove it
     * from the output buffer so another call to getOutput() will
     * not return duplicate data.
     *
     * @returns All data currently in the output buffer.
     */
    HeatshrinkDecoder.prototype.getOutput = function () {
        var size = this.outputSize;
        var output = this.outputBuffer.slice(0, size);
        this.outputBuffer = this.outputBuffer.slice(size);
        this.outputSize = 0;
        return output;
    };
    HeatshrinkDecoder.prototype.assureUint8Array = function (buffer) {
        if (buffer instanceof ArrayBuffer) {
            return new Uint8Array(buffer);
        }
        return buffer;
    };
    HeatshrinkDecoder.prototype.processTag = function () {
        var bit = getBits(1, this.inputBuffer, this.inputState);
        if (bit === HS_NOBITS) {
            return HSState.TAG_BIT;
        }
        else if (bit === HS_LITERAL_MARKER) {
            return HSState.YIELD_LITERAL;
        }
        else if (this.windowBits > 8) {
            return HSState.BACKREF_INDEX_MSB;
        }
        else {
            this.outputIndex = 0;
            return HSState.BACKREF_INDEX_LSB;
        }
    };
    HeatshrinkDecoder.prototype.yieldLiteral = function () {
        var byte = getBits(8, this.inputBuffer, this.inputState);
        if (byte === HS_NOBITS) {
            return HSState.YIELD_LITERAL;
        }
        this.emitByte(byte);
        this.storeByte(byte);
        return HSState.TAG_BIT;
    };
    HeatshrinkDecoder.prototype.processBackrefIndexMSB = function () {
        if (this.windowBits <= 8) {
            throw new HSInternalError("There should not be any index MSB handling when the backref index is <= 8 bits.");
        }
        var msb = getBits(this.windowBits - 8, this.inputBuffer, this.inputState);
        if (msb === HS_NOBITS) {
            return HSState.BACKREF_INDEX_MSB;
        }
        this.outputIndex = msb << 8;
        return HSState.BACKREF_INDEX_LSB;
    };
    HeatshrinkDecoder.prototype.processBackrefIndexLSB = function () {
        var bitCount = this.windowBits;
        if (bitCount > 8) {
            bitCount = 8;
        }
        var lsb = getBits(bitCount, this.inputBuffer, this.inputState);
        if (lsb === HS_NOBITS) {
            return HSState.BACKREF_INDEX_LSB;
        }
        this.outputIndex |= lsb;
        this.outputIndex += 1;
        this.outputCount = 0;
        if (this.lookaheadBits > 8) {
            return HSState.BACKREF_COUNT_MSB;
        }
        return HSState.BACKREF_COUNT_LSB;
    };
    HeatshrinkDecoder.prototype.processBackrefCountMSB = function () {
        if (this.lookaheadBits <= 8) {
            throw new HSInternalError("There should not be any count MSB handling when the backref index is <= 8 bits.");
        }
        var msb = getBits(this.lookaheadBits - 8, this.inputBuffer, this.inputState);
        if (msb === HS_NOBITS) {
            return HSState.BACKREF_COUNT_MSB;
        }
        this.outputCount = msb << 8;
        return HSState.BACKREF_COUNT_LSB;
    };
    HeatshrinkDecoder.prototype.processBackrefCountLSB = function () {
        var bitCount = this.lookaheadBits;
        if (bitCount > 8) {
            bitCount = 8;
        }
        var lsb = getBits(bitCount, this.inputBuffer, this.inputState);
        if (lsb === HS_NOBITS) {
            return HSState.BACKREF_COUNT_LSB;
        }
        this.outputCount |= lsb;
        this.outputCount += 1;
        return HSState.YIELD_BACKREF;
    };
    HeatshrinkDecoder.prototype.yieldBackref = function () {
        var negativeOffset = this.outputIndex;
        if (negativeOffset > this.windowBuffer.byteLength) {
            throw new HSCorruptDataError("A negative offset was received that was larger than our window size.");
        }
        if (this.outputCount > this.windowBuffer.byteLength) {
            throw new HSCorruptDataError("A backreference size was received that was larger than our window size.");
        }
        for (var i = 0; i < this.outputCount; ++i) {
            var index = this.headIndex - negativeOffset;
            if (index < 0) {
                index += this.windowBuffer.byteLength;
            }
            var byte = this.windowBuffer[index];
            this.emitByte(byte);
            this.storeByte(byte);
        }
        return HSState.TAG_BIT;
    };
    HeatshrinkDecoder.prototype.ensureOutputSpace = function (neededBytes) {
        var remaining = this.outputBuffer.byteLength - this.outputSize;
        if (remaining < neededBytes) {
            var newSize = 2 * Math.max(this.outputBuffer.byteLength, 1);
            var newBuffer = new Uint8Array(newSize);
            newBuffer.set(this.outputBuffer.slice(0, this.outputSize));
            this.outputBuffer = newBuffer;
        }
    };
    HeatshrinkDecoder.prototype.emitByte = function (byte) {
        this.ensureOutputSpace(1);
        this.outputBuffer[this.outputSize] = byte;
        this.outputSize += 1;
        var char = String.fromCharCode(byte);
    };
    HeatshrinkDecoder.prototype.storeByte = function (byte) {
        this.windowBuffer[this.headIndex] = byte;
        this.headIndex += 1;
        if (this.headIndex >= this.windowBuffer.byteLength) {
            this.headIndex %= this.windowBuffer.byteLength;
        }
    };
    return HeatshrinkDecoder;
}());

// util
/**
 * @module heatshrink-utils
 * @external
 * @preferred
 *
 * This internal module contains private utility functions that are used inside
 * the heatshrink-ts package.  They are not meant to be used externally nor are
 * they externally visible.
 */

/**
 * Get a specific number of bits from the input buffer.  You can get between
 * 1 and 15 bits at a time and those bits are popped from the input buffer and
 * returned.  If there are not enough bits remaining in buffer according to the
 * state information in state, then HS_NOBITS is returned as a sentinal value
 * and no bits are consumed from teh buffer.
 *
 * @param count The number of bits to return, must be in the range [1, 15]
 * @param state The current state of the input bitstream.  This parameter is
 *    modified with every invocation of this function to maintain state between
 *    calls.
 * @param buffer A buffer of input data that will be used to retrieve a fixed
 *    number of bits.  This parameter is never modified.  The state of what
 *    bits have and have not been extracted is stored in the state parameter.
 * @returns The bits that were popped from the input buffer.  If there are not
 *    enough bits left in the buffer then HS_NOBITS is returned and state is left
 *    unchanged.
 */
function getBits(count, buffer, state) {
    if (count > 15) {
        throw new HSInternalError("getBits called with invalid number of bits requested (" + count + " not in [1, 15])");
    }
    /*
     * Make sure that we have enough available bits to satisfy this call.  There are two cases where
     * we could fail to have enough bits:
     * 1. We are on the last byte and there are fewer bits left than count
     * 2. We are on the penultimate byte and there are fewers bits left in the byte than count - 8
     *    so that when we move to the next byte
     */
    if (state.size === 0 && state.bitIndex < 1 << (count - 1)) {
        return HS_NOBITS;
    }
    else if (state.size - state.index === 1 && count > 8) {
        var requiredBitmask = 1 << (count - 8 - 1);
        if (state.bitIndex < requiredBitmask) {
            return HS_NOBITS;
        }
    }
    var accum = 0;
    for (var i = 0; i < count; ++i) {
        if (state.bitIndex === 0 && state.size === 0) {
            return HS_NOBITS;
        }
        if (state.bitIndex === 0) {
            state.currentByte = buffer[state.index];
            state.index += 1;
            // Keep track of when the inputBuffer is used up and mark it as empty again
            if (state.index === state.size) {
                state.index = 0;
                state.size = 0;
            }
            state.bitIndex = 1 << 7;
        }
        accum <<= 1;
        if (state.currentByte & state.bitIndex) {
            accum |= 1;
        }
        state.bitIndex >>= 1;
    }
    return accum;
}