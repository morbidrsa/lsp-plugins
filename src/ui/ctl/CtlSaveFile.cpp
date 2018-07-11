/*
 * CtlSaveFile.cpp
 *
 *  Created on: 20 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlSaveFile::CtlSaveFile(CtlRegistry *reg, LSPSaveFile *save):
            CtlWidget(reg, save)
        {
//            save->set_state(SFS_SAVING);
//            save->set_progress(40);

            pFile       = NULL;
            pStatus     = NULL;
            pCommand    = NULL;
            pProgress   = NULL;
        }

        CtlSaveFile::~CtlSaveFile()
        {
        }

        void CtlSaveFile::update_state()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save == NULL)
                return;
            if (pStatus == NULL)
                return;

            size_t status = pStatus->get_value();
            if (status == STATUS_UNSPECIFIED)
                save->set_state(SFS_SELECT);
            else if (status == STATUS_OK)
            {
                save->set_state(SFS_SAVED);
                if (pCommand != NULL)
                {
                    pCommand->set_value(0.0f);
                    pCommand->notify_all();
                }
            }
            else if (status == STATUS_LOADING)
            {
                save->set_state(SFS_SAVING);
                if (pProgress != NULL)
                    save->set_progress(pProgress->get_value());
            }
            else
            {
                save->set_state(SFS_ERROR);
                if (pCommand != NULL)
                {
                    pCommand->set_value(0.0f);
                    pCommand->notify_all();
                }
            }
        }

        status_t CtlSaveFile::slot_on_file_submit(LSPWidget *sender, void *ptr, void *data)
        {
            CtlSaveFile *_this   = static_cast<CtlSaveFile *>(ptr);
            return (_this != NULL) ? _this->commit_state() : STATUS_BAD_ARGUMENTS;
        }

        status_t CtlSaveFile::commit_state()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save == NULL)
                return STATUS_OK;

            const char *path = save->file_name();
            if (pFile != NULL)
            {
                pFile->write(path, strlen(path));
                pFile->notify_all();
            }
            if (pCommand != NULL)
            {
                pCommand->set_value(1.0f);
                pCommand->notify_all();
            }

            return STATUS_OK;
        }

        void CtlSaveFile::set(widget_attribute_t att, const char *value)
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pFile, value);
                    break;
                case A_STATUS_ID:
                    BIND_PORT(pRegistry, pStatus, value);
                    break;
                case A_COMMAND_ID:
                    BIND_PORT(pRegistry, pCommand, value);
                    break;
                case A_PROGRESS_ID:
                    BIND_PORT(pRegistry, pProgress, value);
                    break;
                case A_FORMAT:
                    if (save != NULL)
                        parse_file_formats(value, save->filter());
                    break;
                case A_FORMAT_ID:
                    BIND_EXPR(sFormat, value);
                    break;
                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlSaveFile::end()
        {
            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save != NULL)
                save->slots()->bind(LSPSLOT_SUBMIT, slot_on_file_submit, this);

            update_state();
        }

        void CtlSaveFile::notify(CtlPort *port)
        {
            if ((port == pStatus) ||
                (port == pProgress))
                update_state();

            LSPSaveFile *save   = widget_cast<LSPSaveFile>(pWidget);
            if (save == NULL)
                return;

            if (sFormat.valid())
                save->filter()->set_default(sFormat.evaluate());
        }
    } /* namespace ctl */
} /* namespace lsp */
