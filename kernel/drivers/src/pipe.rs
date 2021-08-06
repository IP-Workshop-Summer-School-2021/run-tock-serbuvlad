use kernel::process::{Error, ProcessId};
use kernel::syscall::{CommandReturn, SyscallDriver};
use kernel::ErrorCode;
use kernel::processbuffer::{ReadOnlyProcessBuffer, ReadWriteProcessBuffer, ReadableProcessBuffer, WriteableProcessBuffer};
use kernel::utilities::cells::TakeCell;

use core::cell::Cell;

use kernel::debug;

use array_init::array_init;

pub const DRIVER_NUM: usize = 0xa0002;

const MAX_PIPES: usize = 1;

/*
struct Pipe {
    input: Option<ReadableProcessSlice>,
    output: Option<WriteableProcessSlice>,

    client_in: ProcessId,
    client_out: ProcessId,
}
*/

/*
struct Pipe<'a, 'b> {
    input: TakeCell<'a, ReadableProcessSlice>,
    output: TakeCell<'b, WriteableProcessSlice>,
}

struct PipeManagerState<'a, 'b> {
    pipes: [Option<Pipe<'a, 'b>>; MAX_PIPES],
    map: [usize; MAX_PIPES],
}

pub struct PipeManager<'a, 'b> {
    state: Cell<PipeManagerState<'a, 'b>>,
}
*/

const BUF_SIZE: usize = 512;

struct Pipe {
    buffer: [Cell<u8>; BUF_SIZE],
    buf_len: Cell<usize>,

    in_pid: Cell<Option<ProcessId>>,
    last_read_len: Cell<usize>,

    out_pid: Cell<Option<ProcessId>>,
    last_write_len: Cell<usize>,
}

impl Default for Pipe {
    fn default() -> Self {
        Pipe {
            buffer: array_init(|_| Cell::new(0)),
            buf_len: Cell::new(0),

            in_pid: Cell::new(Option::None),
            last_read_len: Cell::new(0),

            out_pid: Cell::new(Option::None),
            last_write_len: Cell::new(0),
        }
    }
}

pub struct PipeManager {
    pipes: [Pipe; MAX_PIPES],
    map: [Cell<usize>; MAX_PIPES],
}

impl PipeManager {
    pub fn new() -> PipeManager {
        PipeManager {
            pipes: array_init(|_| Pipe::default()),
            map: array_init(|_| Cell::new(0)),
        }
    }

    fn find_pipe(&self, pipe_no: usize, create: bool) -> Option<&Pipe> {
        for i in 0..MAX_PIPES {
            if self.map[i].get() == pipe_no {
                return Option::Some(&self.pipes[i]);
            }
        }

        if !create {
            return Option::None;
        }

        for i in 0..MAX_PIPES {
            if self.map[i].get() == 0 {
                self.map[i].set(pipe_no);

                return Option::Some(&self.pipes[i]);
            }
        }

        Option::None
    }
}

impl SyscallDriver for PipeManager {
    fn command(
        &self,
        command_num: usize,
        r2: usize,
        r3: usize,
        process_id: ProcessId,
    ) -> CommandReturn {
        match command_num {
            0 => CommandReturn::success(),

            // register pipe
            1 => {
                let pipe_no = r2;
                let input = (r3 == 0);

                if pipe_no < 0x100 {
                    return CommandReturn::failure(ErrorCode::INVAL);
                }

                match self.find_pipe(pipe_no, true) {
                    Some(pipe) => {
                        // TODO: verif exista

                        if input {
                            pipe.in_pid.set(Some(process_id));
                        } else {
                            pipe.out_pid.set(Some(process_id));
                        }

                        CommandReturn::success()
                    }
                    None => {
                        CommandReturn::failure(ErrorCode::BUSY)
                    }
                }
            }

            2 => {
                let pipe_no = r2;
                let input = (r3 == 0);

                if pipe_no < 0x100 {
                    return CommandReturn::failure(ErrorCode::INVAL);
                }

                match self.find_pipe(pipe_no, true) {
                    Some(pipe) => {
                        // TODO: verif exista

                        if input {
                            CommandReturn::success_u32(pipe.last_write_len.get() as u32)
                        } else {
                            CommandReturn::success_u32(pipe.last_read_len.get() as u32)
                        }
                    }
                    None => {
                        CommandReturn::failure(ErrorCode::BUSY)
                    }
                }
            }

            _ => CommandReturn::failure(ErrorCode::NOSUPPORT),
        }
    }

    fn allow_readonly(
        &self,
        process_id: ProcessId,
        allow_num: usize,
        slice: ReadOnlyProcessBuffer,
    ) -> Result<ReadOnlyProcessBuffer, (ReadOnlyProcessBuffer, ErrorCode)> {
        match self.find_pipe(allow_num, false) {
            Some(pipe) => {
                match pipe.in_pid.get() {
                    Some(in_pid) => {
                        if in_pid == process_id {
                            match slice.enter(|slice| {

                                let mut pos = pipe.buf_len.get();

                                let first_pos = pos;

                                for i in slice.iter() {
                                    pipe.buffer[pos].set(i.get());

                                    pos += 1;

                                    if pos > BUF_SIZE {
                                        break;
                                    }
                                }

                                pipe.last_write_len.set(pos - first_pos);
                                pipe.buf_len.set(pos);

                                if false {
                                    return Err(())
                                } else {
                                    return Ok(())
                                }
                            }) {
                                Ok(_) => Ok(slice),
                                Err(_) => Err((slice, ErrorCode::BUSY)),
                            }
                        } else {
                            Err((slice, ErrorCode::RESERVE))
                        }
                    }

                    
                    None => Err((slice, ErrorCode::RESERVE))
                }
            }

            
            None => Err((slice, ErrorCode::BUSY))
        }
    }

    fn allow_readwrite(
        &self,
        process_id: ProcessId,
        allow_num: usize,
        slice: ReadWriteProcessBuffer,
    ) -> Result<ReadWriteProcessBuffer, (ReadWriteProcessBuffer, ErrorCode)> {
        match self.find_pipe(allow_num, false) {
            Some(pipe) => {
                match pipe.out_pid.get() {
                    Some(out_pid) => {
                        if out_pid == process_id {
                            match slice.mut_enter(|slice| {

                                let mut pos = 0;

                                for i in slice.iter() {
                                    i.set(pipe.buffer[pos].get());

                                    pos += 1;

                                    if pos == pipe.buf_len.get() {
                                        break;
                                    }
                                }

                                pipe.last_read_len.set(pos);

                                for i in 0..pos {
                                    if pos + i >= pipe.buf_len.get() {
                                        break;
                                    }

                                    pipe.buffer[i].set(pipe.buffer[pos + i].get());
                                }

                                pipe.buf_len.set(pipe.buf_len.get() - pos);

                                if false {
                                    return Err(())
                                } else {
                                    return Ok(())
                                }
                            }) {
                                Ok(_) => Ok(slice),
                                Err(_) => Err((slice, ErrorCode::BUSY)),
                            }
                        } else {
                            Err((slice, ErrorCode::RESERVE))
                        }
                    }

                    None => Err((slice, ErrorCode::RESERVE))
                }
            }
            None => Err((slice, ErrorCode::BUSY))
        }
    }

    fn allocate_grant(&self, _process_id: ProcessId) -> Result<(), Error> {
        Ok(())
    }
}

/*

impl SyscallDriver for PipeManager<'_, '_> {
    fn allocate_grant(&self, _process_id: ProcessId) -> Result<(), Error> {
        Ok(())
    }

    fn command(
        command_num: usize,
        r2: usize,
        _r3: usize,
        _process_id: ProcessId,
    ) -> CommandReturn {
        match command_num {
            0 => CommandReturn::success(),

            1 => self.state.get().pipes.input,

            _ => CommandReturn::failure(ErrorCode::NOSUPPORT),
        }
    }
}fn allow_readwrite(
        &self,
        process_id: ProcessId,
        allow_num: usize,
        slice: ReadWriteProcessBuffer,
    ) -> Result<ReadWriteProcessBuffer, (ReadWriteProcessBuffer, ErrorCode)> {
        Err((slice, ErrorCode::NOSUPPORT))
    }
*/
