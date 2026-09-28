import { isPlatformBrowser } from '@angular/common';
import { DOCUMENT, inject, PLATFORM_ID, Service, signal, WritableSignal } from '@angular/core';


type Theme = 'light' | 'dark';

@Service()
export class ThemeService {
    private readonly m_platformID = inject(PLATFORM_ID);
    private readonly m_theme: WritableSignal<Theme> = signal<Theme>(this.getSavedTheme());
    private m_document = inject(DOCUMENT);

    public get theme(): WritableSignal<Theme> {
        return this.m_theme;
    }

    public saveTheme(): void {
        const current = this.theme();

        if (!isPlatformBrowser(this.m_platformID))
            return;

        localStorage.setItem('devola-theme', current);
        document.documentElement.setAttribute('data-theme', current);
    }

    public toggle(): void {
        this.theme.update((t) => (t === "light" ? "dark" : "light"));
        const htmlElement = this.m_document.documentElement;
        if (this.theme() === "dark")
            htmlElement.setAttribute('data-theme', 'dark');
        else
            htmlElement.removeAttribute('data-theme');
    }

    public getSavedTheme(): Theme {
        if (!isPlatformBrowser(this.m_platformID))
            return "light";

        const stored = localStorage.getItem('devola-theme') as Theme | null;
        if (stored === 'light' || stored === 'dark') return stored;
        return window.matchMedia?.('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
    }
}
